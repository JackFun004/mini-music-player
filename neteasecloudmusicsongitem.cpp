#include "neteasecloudmusicsongitem.h"
#include<QDebug>
#include "songhelper.h"
#include<QJsonObject>
#include<QJsonArray>
// #include "apihelper.h"
NeteaseCloudMusicSongItem::NeteaseCloudMusicSongItem(quint64 id_)
    :SongItem(QUrl(),"default",QUrl()),id(id_)
{
    //获取url
    auto ret = QJsonObject::fromVariantMap(SongHelper::getSongHelper()->apiHelper->invoke("song_url",{{"id",id}}));
    songUrl = ret["body"].toObject()["data"].toArray()[0].toObject()["url"].toString();

    //歌没拿到直接退出
    if(!songUrl.isValid())
        return;

    ret = QJsonObject::fromVariantMap(SongHelper::getSongHelper()->apiHelper->invoke("song_detail",{{"ids",id}}));
    songName = ret["body"].toObject()["songs"].toArray()[0].toObject()["name"].toString();
    imageUrl = ret["body"].toObject()["songs"].toArray()[0].toObject()["al"].toObject()["picUrl"].toString();

}

QString NeteaseCloudMusicSongItem::getSongName() const
{
    return songName;
}

QUrl NeteaseCloudMusicSongItem::getSongUrl() const
{
    return songUrl;
}

QUrl NeteaseCloudMusicSongItem::getImageUrl() const
{
    return imageUrl;
}

QImage NeteaseCloudMusicSongItem::getImage() const
{
    QImage temp;
    return temp;
}
