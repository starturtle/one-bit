#include "ResultImage.h"

ResultImage::ResultImage(QQuickItem* parent)
: QQuickPaintedItem()
, resultImage{}
{}

void ResultImage::setData(const QImage& data)
{
  resultImage = data;
  update();
}
void ResultImage::paint(QPainter* painter){
  QRectF bounds = boundingRect();
  QImage scaled = resultImage.scaledToHeight(bounds.height());
  QPointF center = bounds.center() - scaled.rect().center();

  if(center.x() < 0) center.setX(0);
  if(center.y() < 0) center.setY(0);
  painter->drawImage(center, scaled);
}
QImage ResultImage::data() const
{
  return resultImage;
}
