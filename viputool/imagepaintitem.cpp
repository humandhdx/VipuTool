#include "imagepaintitem.h"


ImagePaintItem::ImagePaintItem(QQuickItem *parent):QQuickPaintedItem(parent)
{

}

void ImagePaintItem::updateImage(const QImage &)
{

}

void ImagePaintItem::recvEmitImg(QImage img)
{
    m_imageThumb = img;
    // emit this->imgChanged();
    update();
}

void ImagePaintItem::paint(QPainter *painter)
{
    painter->drawImage(this->boundingRect(), m_imageThumb);
}
