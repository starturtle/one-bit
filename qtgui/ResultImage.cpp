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
    logging::LogStream::instance().getLogStream(logging::Level::WARNING) << "Input Image empty!" << std::endl;
  }
  else
  {
    logging::LogStream::instance().getLogStream(logging::Level::INFO) << "Setting Result Image" << std::endl;
  }
  image = data;
  if (image.isNull())
  {
    logging::LogStream::instance().getLogStream(logging::Level::WARNING) << "Result Image empty!" << std::endl;
  }
  else
  {
    logging::LogStream::instance().getLogStream(logging::Level::INFO) << "Set Result Image" << std::endl;
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