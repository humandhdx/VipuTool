#ifndef IMAGEPAINTITEM_H
#define IMAGEPAINTITEM_H

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>

class ImagePaintItem : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ImagePaintItem(QQuickItem *parent = nullptr);

public slots:
    void updateImage(const QImage &);
    void recvEmitImg(QImage img);

protected:
    void paint(QPainter *painter) override;

private:
    QImage m_imageThumb;

signals:
    void imgChanged();
};

#endif // IMAGEPAINTITEM_H
