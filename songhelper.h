#ifndef SONGHELPER_H
#define SONGHELPER_H

#include <QObject>
#include <QQmlEngine>
#include "songitem.h"
#include <QHotkey>
#include <QAudioBufferOutput>
#include "apihelper.h"
#include <QRunnable>
#include <QThread>
#include <QSystemTrayIcon>

class SongHelper : public QObject
{
    Q_OBJECT
public:
    static QObject* singletonProvider(QQmlEngine *engine, QJSEngine *scriptEngine);
    static SongHelper* getSongHelper();

    explicit SongHelper(QObject *parent = nullptr);
    ~SongHelper();


    Q_INVOKABLE void generateSongItems(QList<QUrl> urls);
    Q_INVOKABLE void searchSongs(QString keywords);//网易云
    Q_INVOKABLE void quitSearchResults();
    Q_INVOKABLE void downloadSongItem(int index);

    Q_INVOKABLE QString getSongName(int index);
    Q_INVOKABLE QUrl getSongUrl(int index);
    Q_INVOKABLE QUrl getImageUrl(int index);
    Q_INVOKABLE void removeSongItem(int index);
    Q_INVOKABLE QAudioBufferOutput* getAudioBufferOutput();

public:
    QImage getImage(int index);
    std::unique_ptr<ApiHelper> apiHelper;

signals:
    void newSongNamesReady(QList<QString>);
    void lastSongKeysPressed();
    void nextSongKeysPressed();
    void playingStateSwitchKeysPressed();
    void mainColorChanged(QColor);
    void searchResultsReady(QList<QString>);

private:
    QList<SongItem*> songItems;
    QList<SongItem*> searchResults;
    bool usingSearchResults;

    static SongHelper* songHelper;

    QHotkey keysLast;
    QHotkey keysNext;
    QHotkey keysSwitchPlayingState;

private:
    QAudioBufferOutput qAudioBufferOutput;
private:
    QImage defaultImg;
    SongItem* generateSongItem(QUrl url);

    class CreateOnlineItem : public QRunnable
    {
    public:
        explicit CreateOnlineItem(QList<SongItem*>& lists,int i,qint64 index);
        void run()override;

    private:
        QList<SongItem*> &lists;
        int i;
        qint64 index;
    };

    class SearchOnlineThread : public QThread
    {
    public:
        SearchOnlineThread(SongHelper*,QString);
        void run()override;
    private:
        SongHelper* sh;
        QString keywords;
    };
private:
    QSystemTrayIcon *trayIcon;
};

#endif // SONGHELPER_H
