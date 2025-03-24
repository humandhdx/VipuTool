#include "imageprovider.h"
#include <iostream>
#include <QTime>
#include <QDir>

ImageProvider::ImageProvider() :QQuickImageProvider(QQuickImageProvider::Image)
{

}
#include <QThread>
ImageProvider::~ImageProvider()
{
    qWarning() << __FUNCTION__ << " - 0!";
    QThread::sleep(2);
}

QImage ImageProvider::requestImage([[maybe_unused]]const QString &id, [[maybe_unused]]QSize *size, [[maybe_unused]]const QSize &requestedSize)
{
    // QStringList parts=id.split('_');
    // if(parts.size()>1){
    //     type=parts[0];
    //     if(type=="leftrect"){
    //         QRect left_rect(0,0,img_.width()/2,img_.height());
    //         left_image_= img_.copy(left_rect);
    //         return left_image_;
    //     }
    //     else if(type=="rightrect"){
    //         QRect  right_rect(992,0,img_.width()/2,img_.height());
    //         right_image_= img_.copy(right_rect);
    //         return right_image_;
    //     }
    //     else if(type=="middlerect"){
    //         right_image_= img_.copy(left_rect);
    //         return right_image_;
    //     }
    // }
    return this->img_;
}

void ImageProvider::captureImage()
{
    // if(!img_.isNull()){
    //     QString fileName = "recv_.jpg";
    //     QString filePath = QDir::currentPath() + "/" + fileName;
    //     img_.save(filePath);
    // }
    // if(!left_image_.isNull()){
    //     QString fileName = type+".jpg";
    //     QString filePath = QDir::currentPath() + "/" + fileName;
    //     left_image_.save(filePath);
    // }
    // if(!right_image_.isNull()){
    //     QString fileName = type +".jpg";
    //     QString filePath = QDir::currentPath() + "/" + fileName;
    //     right_image_.save(filePath);
    // }
}
void ImageProvider::recvEmitImg(QImage img){
    img_ = img;
    emit this->imgChanged();
}

