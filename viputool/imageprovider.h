#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QTimer>

class ImageProvider : public QQuickImageProvider
{
    Q_OBJECT
public:
    ImageProvider();
    ~ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize);


public slots:
    void recvEmitImg(QImage img);
    void captureImage();

signals:
    void imgChanged();

private:
    QImage img_;
    QString type;

};

#endif // IMAGEPROVIDER_H
