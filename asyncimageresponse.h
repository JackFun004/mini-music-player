#ifndef ASYNCIMAGERESPONSE_H
#define ASYNCIMAGERESPONSE_H

#include <QObject>
#include <QQuickImageResponse>
#include <QRunnable>

class AsyncImageResponse : public QQuickImageResponse, public QRunnable {
    Q_OBJECT
public:
    static QImage defaultImg;

    AsyncImageResponse(const QString &url, const QSize &requestedSize);
    ~AsyncImageResponse();

    QQuickTextureFactory *textureFactory() const override;

    void run() override;
signals:
    void mainColorChanged(QColor);

private:
    QColor getMainColor(const QImage&);
    void taskFinish(const QImage&);
private:
    QString id;
    QSize size;
    QImage m_image;

    QNetworkAccessManager *netManager;

};


#endif // ASYNCIMAGERESPONSE_H
