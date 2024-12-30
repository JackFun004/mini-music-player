#include "songitem.h"


QString SongItem::getSongTitle() const
{
    return this->getSongName();
}

SongItem::SongItem(QUrl url, QString name, QUrl image)
    :songUrl(url),songName(name),imageUrl(image)
{

}

SongItem::~SongItem()
{}
