#include "songhelper.h"
#include "localsongitem.h"
#include "neteasecloudmusicsongitem.h"
#include <QAudioBuffer>
#include <QJsonObject>
#include <QJsonArray>
#include <QThreadPool>
#include <QMenu>

SongHelper* SongHelper::songHelper = nullptr;
QObject *SongHelper::singletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    if(SongHelper::songHelper==nullptr)
        SongHelper::songHelper=new SongHelper();
    return SongHelper::songHelper;
}

SongHelper* SongHelper::getSongHelper()
{
    if(SongHelper::songHelper==nullptr)
        SongHelper::songHelper=new SongHelper();
    return SongHelper::songHelper;
}

SongHelper::SongHelper(QObject *parent)
    : QObject{parent},
    apiHelper(std::make_unique<ApiHelper>()),
    usingSearchResults(false),
    keysLast(QKeySequence("Ctrl+Alt+Left"),true,this),
    keysNext(QKeySequence("Ctrl+Alt+Right"),true,this),
    keysSwitchPlayingState(QKeySequence("Ctrl+Alt+P"),true,this),
    qAudioBufferOutput(this),
    defaultImg(":/assets/disk.jpeg")
{
    // trayIcon = new QSystemTrayIcon(QIcon(":/assets/disk.jpeg"),this);
    // QMenu *menu = new QMenu();


    QObject::connect(&keysLast,&QHotkey::activated,this,[&](){emit lastSongKeysPressed();});
    QObject::connect(&keysNext,&QHotkey::activated,this,[&](){emit nextSongKeysPressed();});
    QObject::connect(&keysSwitchPlayingState,&QHotkey::activated,this,[&](){emit playingStateSwitchKeysPressed();});

    //游客登录网易云
    // apiHelper->invoke("register_anonimous",{});//似乎不必要

}

SongHelper::~SongHelper()
{
    for(int i=0;i<songItems.size();i++){
        delete songItems[i];
    }
    for(int i=0;i<searchResults.size();i++){
        delete searchResults[i];
    }
}

SongItem* SongHelper::generateSongItem(QUrl url)
{
    if(!url.isValid())
        return nullptr;
    if(url.isLocalFile()){
        return new LocalSongItem(url);
    }
    return nullptr;
}

void SongHelper::generateSongItems(QList<QUrl> urls)
{
    QList<QString> newSongNames;
    for(auto& url:urls){
        SongItem* temp = generateSongItem(url);
        if(temp){
            songItems.append(temp);
            newSongNames.append(songItems.last()->getSongName());
        }

    }
    emit newSongNamesReady(newSongNames);
}

void SongHelper::searchSongs(QString keywords)
{
    auto thread = new SearchOnlineThread(this,keywords);
    qDebug()<<"开始搜索： "<<keywords;
    thread->start();

}

void SongHelper::quitSearchResults()
{
    usingSearchResults = false;
}

void SongHelper::downloadSongItem(int index)
{
    if(!usingSearchResults)
        return;
    if(index<0||index>=searchResults.length())
        return;
    auto item =dynamic_cast<NeteaseCloudMusicSongItem*>(searchResults[index]);
    if(item){
        //to do
        //download

    }
}

QString SongHelper::getSongName(int index)
{
    if(usingSearchResults){
        if(index>=0&&index<searchResults.length()){
            return searchResults[index]->getSongName();
        }
        return QString();
    }
    if(index>=0&&index<songItems.length()){
        return songItems[index]->getSongName();
    }
    return QString();
}

QUrl SongHelper::getSongUrl(int index)
{
    if(usingSearchResults){
        if(index>=0&&index<searchResults.length()){
            return searchResults[index]->getSongUrl();
        }
        return QUrl();
    }
    if(index>=0&&index<songItems.length()){
        return songItems[index]->getSongUrl();
    }
    return QUrl();
}

QUrl SongHelper::getImageUrl(int index)
{
    if(usingSearchResults){
        if(index>=0&&index<searchResults.length()){
            return searchResults[index]->getImageUrl();
        }
        return QUrl();
    }
    if(index>=0&&index<songItems.length()){
        return songItems[index]->getImageUrl();
    }
    return QUrl();
}

void SongHelper::removeSongItem(int index)
{
    if(usingSearchResults)
        return;
    if(index>=0&&index<songItems.length()){
        SongItem* temp=songItems[index];
        songItems.remove(index);
        delete temp;
    }
}

QAudioBufferOutput *SongHelper::getAudioBufferOutput()
{
    return &qAudioBufferOutput;
}

QImage SongHelper::getImage(int index)
{
    if(usingSearchResults){
        if(index>=0&&index<searchResults.length()){
            return searchResults[index]->getImage();
        }
        return QImage();
    }
    if(index>=0&&index<songItems.length()){
        return songItems[index]->getImage();
    }
    return QImage();
}


SongHelper::CreateOnlineItem::CreateOnlineItem(QList<SongItem *> &lists_, int i_, qint64 index_)
    :lists(lists_),i(i_),index(index_)
{

}

void SongHelper::CreateOnlineItem::run()
{
    lists[i] = new NeteaseCloudMusicSongItem(index);
}

SongHelper::SearchOnlineThread::SearchOnlineThread(SongHelper *sh_, QString kw)
    :sh(sh_),keywords(kw)
{
}

void SongHelper::SearchOnlineThread::run()
{
    auto jsonObject = QJsonObject::fromVariantMap(sh->apiHelper->invoke("search",{{"keywords",keywords}}));
    auto songs = jsonObject["body"].toObject()["result"].toObject()["songs"].toArray();

    for(int i=0;i<sh->searchResults.size();i++){
        delete sh->searchResults[i];
    }
    sh->searchResults.clear();
    sh->searchResults.assign(songs.size(),nullptr);
    if(QThreadPool::globalInstance()->maxThreadCount()<songs.size())
        QThreadPool::globalInstance()->setMaxThreadCount(songs.size());

    for(int i=0;i<songs.size();i++){
        QJsonObject song = songs[i].toObject();
        CreateOnlineItem *r = new CreateOnlineItem(sh->searchResults,i,song["id"].toInteger());
        QThreadPool::globalInstance()->start(r);
    }
    qDebug()<<"线程全部开始执行";
    QThreadPool::globalInstance()->waitForDone(-1);
    qDebug()<<"线程全部执行完毕";

    //剔除无效url
    for(int i=sh->searchResults.size()-1;i>=0;i--){
        if(!(sh->searchResults[i]->getSongUrl().isValid())){
            delete sh->searchResults[i];
            sh->searchResults.remove(i);
        }
    }
    QList<QString> newSongNames;
    for(int i=0;i<sh->searchResults.size();i++){
        newSongNames.append(sh->searchResults[i]->getSongName());
    }
    sh->usingSearchResults = true;
    emit sh->searchResultsReady(newSongNames);
    this->deleteLater();
}
