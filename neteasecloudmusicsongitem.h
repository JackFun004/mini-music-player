#ifndef NETEASECLOUDMUSICSONGITEM_H
#define NETEASECLOUDMUSICSONGITEM_H

#include "songitem.h"

class NeteaseCloudMusicSongItem : public SongItem
{
public:
    explicit NeteaseCloudMusicSongItem(quint64 id);

public:
    QString getSongName() const override;

    QUrl getSongUrl() const override;

    QUrl getImageUrl() const override;

    QImage getImage() const override;
private:
    quint64 id;
};

#endif // NETEASECLOUDMUSICSONGITEM_H
