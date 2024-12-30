#include "asyncimageresponse.h"
#include "songhelper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
QImage AsyncImageResponse::defaultImg;

QColor AsyncImageResponse::getMainColor(const QImage &image)
{
    if(image.isNull())
        return QColor::fromString("#323232");
    quint64 r=0,g=0,b=0;
    int cnt = 0;
    for(int i=0;i<image.height();i+=20){
        for(int j=0;j<image.width();j+=20){
            cnt++;
            QRgb rgb = image.pixel(i,j);
            r += qRed(rgb);
            g += qGreen(rgb);
            b += qBlue(rgb);
        }
    }
    r = r/cnt;
    g = g/cnt;
    b = b/cnt;
    return QColor::fromRgb(r,g,b);
}

void AsyncImageResponse::taskFinish(const QImage &temp)
{
    if(temp.isNull()){
        this->m_image = AsyncImageResponse::defaultImg;
        emit mainColorChanged(QColor::fromString("#323232"));
        emit finished();
        return;
    }

    this->m_image = temp;
    QColor mainColor = getMainColor(temp);
    emit mainColorChanged(mainColor);
    emit finished();

}



AsyncImageResponse::AsyncImageResponse(const QString &url, const QSize &requestedSize)
    :id(url),size(requestedSize),netManager(nullptr)
{
    //finished()发出后会通过deleteLater()自动删除，故需要阻止线程池自动删除Runnable实例
    setAutoDelete(false);

    qDebug()<<"[AsyncImageResponse]triggered in AsyncImageResponse";
    if(AsyncImageResponse::defaultImg.isNull())
        AsyncImageResponse::defaultImg.load(":/assets/disk.jpeg");
    QObject::connect(this,&AsyncImageResponse::mainColorChanged,this,[=](QColor color){
        emit SongHelper::getSongHelper()->mainColorChanged(color);
    });
}

AsyncImageResponse::~AsyncImageResponse()
{
    if(netManager){
        qDebug()<<"delete netManager";
        delete netManager;
    }
}

QQuickTextureFactory *AsyncImageResponse::textureFactory() const
{
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

void AsyncImageResponse::run()
{

    bool ok;
    int id_t = id.toInt(&ok);
    if(ok&&id_t>=0){

        QImage temp = SongHelper::getSongHelper()->getImage(id.toInt());
        if(temp.isNull()){
            QUrl imageUrl = SongHelper::getSongHelper()->getImageUrl(id.toInt());
            if(imageUrl.isValid()){
                QEventLoop *loop = new QEventLoop();
                netManager = new QNetworkAccessManager();
                QObject::connect(netManager,&QNetworkAccessManager::finished,this,[=](QNetworkReply*reply){
                    if(reply->error()==QNetworkReply::NoError){
                        qDebug()<<"图片资源请求成功";
                        QByteArray bytes = reply->readAll();
                        QImage temp = QImage::fromData(bytes);
                        this->taskFinish(temp);
                    }else{
                        qDebug()<<"图片资源请求错误："<<reply->errorString();
                        this->taskFinish(QImage());
                    }
                    reply->deleteLater();
                    loop->quit();
                });
                QNetworkRequest req(imageUrl);
                qDebug()<<"[AsyncImageResponse::run]before request";
                netManager->get(req);
                //通过QEventLoop等待finished信号从而返回图片
                loop->exec();
                qDebug()<<"end loop";
                delete loop;
            }else{
                taskFinish(QImage());
            }
        }else{
            taskFinish(temp);
        }
    }else{
        taskFinish(QImage());
    }

}
