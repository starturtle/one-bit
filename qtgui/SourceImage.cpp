#include "SourceImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

SourceImage::SourceImage(QQuickItem* parent)
: QQuickPaintedItem()
, filePath{}
, image{}
, topLeft{ 0, 0 }
, bottomRight{ 0, 0 }
, newTopLeft{ -1, -1 }
, newBottomRight{ -1, -1 }
, abort{ false }
{
  logging::LogStream::instance().setLogLevel(logging::Level::DEBUG);
  setAcceptedMouseButtons(Qt::AllButtons);
}

void SourceImage::mousePressEvent(QMouseEvent* theEvent)
{
  newTopLeft = { theEvent->localPos().x(), theEvent->localPos().y() };
  newBottomRight = newTopLeft;
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MousePress: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << std::endl;
}

void SourceImage::mouseMoveEvent(QMouseEvent* theEvent)
{
  if (theEvent->buttons() & Qt::MouseButton::LeftButton)
  {
    QPointF tl = { std::min(newTopLeft.x(), theEvent->localPos().x()), std::min(newTopLeft.y(), theEvent->localPos().y()) };
    QPointF br = { std::max(newTopLeft.x(), theEvent->localPos().x()), std::max(newTopLeft.y(), theEvent->localPos().y()) };
    newTopLeft = tl;
    newBottomRight = br;
  }
  abort = (theEvent->buttons() & Qt::MouseButton::RightButton);
  if (! abort)
  {
    update();
  }
}

void SourceImage::mouseReleaseEvent(QMouseEvent* theEvent)
{
  if (abort)
  {
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MouseRelease (aborted)" << std::endl;
  }
  else
  {
    QPointF tl = { std::min(newTopLeft.x(), theEvent->localPos().x()), std::min(newTopLeft.y(), theEvent->localPos().y()) };
    QPointF br = { std::max(newTopLeft.x(), theEvent->localPos().x()), std::max(newTopLeft.y(), theEvent->localPos().y()) };
    topLeft = tl;
    bottomRight = br;
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MouseRelease: top left is " << topLeft.x() << ", " << topLeft.y() << ", bottom right is " << bottomRight.x() << ", " << bottomRight.y() << std::endl;
  }
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  update();
}

void SourceImage::setPath(const QUrl& data)
{
  filePath = data;
  image.load(data.toLocalFile());
  topLeft = { 0, 0 };
  bottomRight = { 0, 0 };
  newTopLeft = { -1, -1 };
  update();
}

void SourceImage::paint(QPainter* painter) {
  QRectF bounds = boundingRect();
  if (image.isNull())
  {
    painter->fillRect(bounds, Qt::white);
    return;
  }
  QImage scaled = image.scaledToWidth(bounds.width());
  QPointF center = bounds.center() - scaled.rect().center();

  if (center.x() < 0)
    center.setX(0);
  if (center.y() < 0)
    center.setY(0);
  setHeight(scaled.height());
  painter->drawImage(center, scaled);
  painter->setPen(QColor(255, 0, 0));
  painter->drawRect(topLeft.x(), topLeft.y(), bottomRight.x()-topLeft.x(), bottomRight.y()-topLeft.y());
  painter->setPen(QColor(0, 255, 0));
  painter->drawRect(newTopLeft.x(), newTopLeft.y(), newBottomRight.x() - newTopLeft.x(), newBottomRight.y() - newTopLeft.y());
}

QImage SourceImage::data() const
{
  return image;
}
