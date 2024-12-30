#ifndef SONGATTACHEDPICPROVIDER_H
#define SONGATTACHEDPICPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QRunnable>
#include <QThreadPool>

class SongAttachedPicProvider : public QQuickAsyncImageProvider
{
    Q_OBJECT
public:
    SongAttachedPicProvider();
    // QImage requestImage(const QString&id,   QSize *, const QSize &) override;
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;

private:
    QThreadPool pool;

};

#endif // SONGATTACHEDPICPROVIDER_H
