#include "ResultImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

ResultImage::ResultImage(QQuickItem* parent)
: QQuickPaintedItem()
, image{}
{}

void ResultImage::setData(const QImage& data)
{
  if (image.isNull())
  {
    logging::LogStream::instance().getLogStream(logging::Level::WARNING) << "Result Image empty!" << std::endl;
  }
  image = data;
  update();
}

void ResultImage::paint(QPainter* painter){
  QRectF bounds = boundingRect();
  if(image.isNull())
  {
    painter->fillRect(bounds, Qt::green);
    return;
  }
  QImage scaled = image.scaledToHeight(bounds.height());
  QPointF center = bounds.center() - scaled.rect().center();

  if(center.x() < 0) center.setX(0);
  if(center.y() < 0) center.setY(0);
  painter->drawImage(center, scaled);
}

QImage ResultImage::data() const
{
  return image;
}