#include "songattachedpicprovider.h"
#include "asyncimageresponse.h"

SongAttachedPicProvider::SongAttachedPicProvider()
    :QQuickAsyncImageProvider(),
    pool(this)
{
    if(AsyncImageResponse::defaultImg.isNull()){
        AsyncImageResponse::defaultImg.load(":/assets/disk.jpeg");
    }
    pool.setMaxThreadCount(5);
}

QQuickImageResponse *SongAttachedPicProvider::requestImageResponse(const QString &id, const QSize &requestedSize)
{
    auto res = new AsyncImageResponse(id, requestedSize);
    pool.start(res);
    return res;
}


