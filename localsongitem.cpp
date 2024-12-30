#include "localsongitem.h"
#include<QDebug>
extern "C"
{
#include <libavcodec/avcodec.h>   //编码
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>  //封装格式处理
#include <libavutil/error.h>
#include <libswscale/swscale.h>  //像素处理
#include <libswresample/swresample.h>  //缩放
}

LocalSongItem::LocalSongItem(QUrl url)
    :SongItem(url,url.fileName(),url),defaultImg(":/assets/disk.jpeg")
{
}

QString LocalSongItem::getSongName() const
{
    return songName;
}


QUrl LocalSongItem::getSongUrl() const
{
    return songUrl;
}

QUrl LocalSongItem::getImageUrl() const
{
    return imageUrl;
}

QImage LocalSongItem::getImage() const
{
    AVFormatContext *m_AVFormatContext = avformat_alloc_context();
    QString file = getImageUrl().path();
    qDebug()<<"[LocalSongItem::getImage]file: "<<file;
    // 打开文件
    int result = avformat_open_input(&m_AVFormatContext, file.mid(1).toStdString().c_str(), nullptr, nullptr);
    qDebug()<<"get open result";
    if (result != 0 || m_AVFormatContext == nullptr){
        qDebug()<<"open failed";
        avformat_free_context(m_AVFormatContext);
        return defaultImg;
    }

    // 查找流信息，把它存入AVFormatContext中
    if (avformat_find_stream_info(m_AVFormatContext, nullptr) < 0){
        qDebug()<<"find no stream_info";
        avformat_free_context(m_AVFormatContext);
        return defaultImg;
    }

    int streamsCount = m_AVFormatContext->nb_streams;
    for (int i=0; i<streamsCount; ++i)
    {
        if (m_AVFormatContext->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)
        {
            qDebug()<<"found attached pic";
            AVPacket pkt = m_AVFormatContext->streams[i]->attached_pic;
            QImage temp = QImage::fromData((uchar*)pkt.data, pkt.size);
            avformat_free_context(m_AVFormatContext);
            return temp;
        }
    }
    qDebug()<<"no attached pic";
    avformat_free_context(m_AVFormatContext);
    return defaultImg;
}
