#ifndef SONGITEM_H
#define SONGITEM_H

#include <QString>
#include <QUrl>
#include <QImage>

class SongItem
{
public:
    explicit SongItem(QUrl url,QString name,QUrl image);
    virtual ~SongItem();

    virtual QString getSongName() const = 0;
    virtual QUrl getSongUrl() const = 0;
    QString getSongTitle() const;
    virtual QUrl getImageUrl() const = 0;
    virtual QImage getImage() const = 0;

signals:

protected:
    QUrl songUrl;
    QString songName;
    QUrl imageUrl;

protected:
    QString& songTitle = songName;

};

#endif // SONGITEM_H
