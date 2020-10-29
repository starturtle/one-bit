#include "ResultImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

ResultImage::ResultImage(QQuickItem* parent)
: QQuickPaintedItem()
, image{}
{
}

void ResultImage::setData(const QImage& data)
{
  if (data.isNull())
  {
    logging::LogStream::instance() << logging::Level::NOTE << "Input Image empty!" << logging::Level::OFF;
  }
  else
  {
    logging::LogStream::instance() << logging::Level::DEBUG << "Setting Result Image" << logging::Level::OFF;
  }
  image = data;
  if (image.isNull())
  {
    logging::LogStream::instance() << logging::Level::WARNING << "Result Image empty!" << logging::Level::OFF;
  }
  else
  {
    logging::LogStream::instance() << logging::Level::DEBUG << "Set Result Image" << logging::Level::OFF;
  }
  update();
}

void ResultImage::paint(QPainter* painter){
  QRectF bounds = boundingRect();
  if(image.isNull())
  {
    painter->fillRect(bounds, Qt::green);
    return;
  }
  QImage scaled = image.scaledToWidth(bounds.width());
  QPointF center = bounds.center() - scaled.rect().center();

  if(center.x() < 0) center.setX(0);
  if(center.y() < 0) center.setY(0);
  painter->drawImage(center, scaled);
}

QImage ResultImage::data() const
{
  return image;
}