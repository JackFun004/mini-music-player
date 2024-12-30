#ifndef LOCALSONGITEM_H
#define LOCALSONGITEM_H

#include "songitem.h"

class LocalSongItem : public SongItem
{
public:
    explicit LocalSongItem(QUrl url);

public:

    QString getSongName() const override;

    QUrl getSongUrl() const override;

    QUrl getImageUrl() const override;

    QImage getImage() const override;
private:
    QImage defaultImg;
};

#endif // LOCALSONGITEM_H
