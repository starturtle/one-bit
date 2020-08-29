#include "SourceImage.h"

#include <algorithm>
#include <QMouseEvent>

SourceImage::SourceImage(QQuickItem* parent)
: QQuickPaintedItem()
, filePath{}
, image{}
, topLeft{ 0, 0 }
, bottomRight{ 0, 0 }
, abort{ false }
{}

void SourceImage::mousePressEvent(QMouseEvent* theEvent)
{
  newTopLeft = { theEvent->localPos().x(), theEvent->localPos().y() };
  newBottomRight = newTopLeft;
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
  if (!abort)
  {
    update();
  }
}

void SourceImage::mouseReleaseEvent(QMouseEvent* theEvent)
{
  if (! abort)
  {
    QPointF tl = { std::min(newTopLeft.x(), theEvent->localPos().x()), std::min(newTopLeft.y(), theEvent->localPos().y()) };
    QPointF br = { std::max(newTopLeft.x(), theEvent->localPos().x()), std::max(newTopLeft.y(), theEvent->localPos().y()) };
    topLeft = tl;
    bottomRight = br;
  }
  update();
}

void SourceImage::setPath(const QUrl& data)
{
  filePath = data;
  image.load(data.toLocalFile());
  topLeft = { 0, 0 };
  bottomRight = { 0, 0 };
  update();
}

void SourceImage::paint(QPainter* painter) {
  QRectF bounding_rect = boundingRect();
  if (image.isNull())
  {
    painter->fillRect(bounding_rect, Qt::white);
    return;
  }
  QImage scaled = image.scaledToWidth(bounding_rect.width());
  QPointF center = bounding_rect.center() - scaled.rect().center();

  if (center.x() < 0)
    center.setX(0);
  if (center.y() < 0)
    center.setY(0);
  setHeight(scaled.height());
  painter->drawImage(center, scaled);
  painter->setPen(QColor(255, 0, 0));
  painter->drawRect(topLeft.x(), topLeft.y(), bottomRight.x()-topLeft.x(), bottomRight.y()-topLeft.y());
}

QImage SourceImage::data() const
{
  return image;
}
