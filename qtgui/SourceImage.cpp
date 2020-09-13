#include "SourceImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

namespace
{
  qreal boundedMin(qreal val1, qreal val2, qreal boundary);
  qreal boundedMax(qreal val1, qreal val2, qreal boundary);
  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, double targetAspectRatio, const QRectF& bounds);
}

SourceImage::SourceImage(QQuickItem* parent)
: QQuickPaintedItem()
, filePath{}
, resultSize{}
, clipTopLeft{}
, clipBottomRight{}
, image{}
, topLeft{ 0, 0 }
, bottomRight{ 0, 0 }
, newStartingPoint{ -1, -1 }
, newTopLeft{ -1, -1 }
, newBottomRight{ -1, -1 }
, abort{ false }
{
  logging::LogStream::instance().setLogLevel(logging::Level::DEBUG);
  setAcceptedMouseButtons(Qt::AllButtons);
}

void SourceImage::mousePressEvent(QMouseEvent* theEvent)
{
  newStartingPoint = { theEvent->localPos().x(), theEvent->localPos().y() };
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MousePress: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << std::endl;
}

void SourceImage::mouseMoveEvent(QMouseEvent* theEvent)
{
  if (theEvent->buttons() & Qt::MouseButton::LeftButton)
  {
    QRectF bounds = boundingRect();
    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MouseMove: boundingRect " << bounds.width() << ", " << bounds.height() << std::endl;
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height() - 1) };
    newTopLeft = tl;
    newBottomRight = br;
    abort = (theEvent->buttons() & Qt::MouseButton::RightButton);
    if (! abort)
    {
      logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MouseMove: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << ", bottom right is " << newBottomRight.x() << ", " << newBottomRight.y() << std::endl;
      update();
    }
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
    QRectF bounds = boundingRect();
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height()) };
    topLeft = tl;
    bottomRight = br;

    logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "MouseRelease: top left is " << topLeft.x() << ", " << topLeft.y() << ", bottom right is " << bottomRight.x() << ", " << bottomRight.y() << std::endl;
  }
  newStartingPoint = { -1, -1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  update();
}

void SourceImage::setPath(const QUrl& data)
{
  filePath = data;
  image.load(data.toLocalFile());
  const std::string fileQuality{ image.isNull() ? "empty " : "" };
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Loaded a new " << fileQuality << "file" << std::endl;
  topLeft = { 0, 0 };
  QRectF bounds = boundingRect();
  bottomRight = { bounds.width(), bounds.height() };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  clipTopLeft = { 0, 0 };
  clipBottomRight = { image.width(), image.height() };
  update();
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "File Size is " << image.width() << "x" << image.height() << std::endl;
}

void SourceImage::setResultWidth(const int& width)
{
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "new width: " << width << std::endl;
  resultSize.setX(width);
  update();
}

void SourceImage::setResultHeight(const int& height)
{
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "new height: " << height << std::endl;
  resultSize.setY(height);
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

  normalizeLocations();
  painter->setPen(QColor(255, 0, 0));
  painter->drawRect(topLeft.x(), topLeft.y(), bottomRight.x()-topLeft.x(), bottomRight.y()-topLeft.y());
  painter->setPen(QColor(0, 255, 0));
  painter->drawRect(newTopLeft.x(), newTopLeft.y(), newBottomRight.x() - newTopLeft.x(), newBottomRight.y() - newTopLeft.y());
}

QImage SourceImage::data() const
{
  auto returnValue{ image.copy(QRect(clipTopLeft, clipBottomRight)) };
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Clipping to (" << clipTopLeft.x() << ", " << clipTopLeft.y() << ")/(" << clipBottomRight.x() << ", " << clipBottomRight.y() << ")" << std::endl;
  const std::string outputIsValid{ returnValue.isNull() ? "empty " : "" };
  const std::string inputIsValid{ image.isNull() ? "empty " : "" };
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Returning " << outputIsValid << "file copied from " << inputIsValid << "input" << std::endl;
  return returnValue;
}

int SourceImage::clipWidth() const
{
  return clipBottomRight.x() - clipTopLeft.x();
}

int SourceImage::clipHeight() const
{
  return clipBottomRight.y() - clipTopLeft.y();
}

int SourceImage::clipX() const
{
  return clipTopLeft.x();
}

int SourceImage::clipY() const
{
  return clipTopLeft.y();
}

void SourceImage::normalizeLocations()
{
  double definedAspectRatio = (1. * resultSize.y()) / resultSize.x();

  QRectF bounds = boundingRect();

  adjustToAspectRatio(topLeft, bottomRight, definedAspectRatio, bounds);
  adjustToAspectRatio(newTopLeft, newBottomRight, definedAspectRatio, bounds);

  clipTopLeft.setX(topLeft.x() * image.width() / bounds.width());
  clipTopLeft.setY(topLeft.y() * image.height() / bounds.height());
  clipBottomRight.setX(bottomRight.x() * image.width() / bounds.width());
  clipBottomRight.setY(bottomRight.y() * image.height() / bounds.height());
}

namespace
{
  qreal boundedMin(qreal val1, qreal val2, qreal boundary)
  {
    return std::max(std::min(val1, val2), boundary);
  }
  qreal boundedMax(qreal val1, qreal val2, qreal boundary)
  {
    return std::min(std::max(val1, val2), boundary);
  }
  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, double targetAspectRatio, const QRectF& bounds)
  {
    double actualHeight = bottomRight.y() - topLeft.y();
    double actualWidth = bottomRight.x() - topLeft.x();
    double actualAspectRatio =  actualHeight / actualWidth;
    static const double aspectRatioMaxDelta{ 0.02 };

    if (std::abs(targetAspectRatio - actualAspectRatio) < aspectRatioMaxDelta)
    {
      return;
    }

    // adjust the smaller one
    if (actualWidth > actualHeight)
    {
      qreal newBottomRightY{ topLeft.y() + actualWidth * targetAspectRatio };
      if (newBottomRightY < bounds.height())
      {
        bottomRight.setY(newBottomRightY);
      }
      else
      {
        bottomRight.setY(bounds.height() - 1);
        actualHeight = bottomRight.y() - topLeft.y();
        bottomRight.setX(topLeft.x() + actualHeight / targetAspectRatio);
      }
    }
    else
    {
      qreal newBottomRightX{ topLeft.x() + actualHeight / targetAspectRatio };
      if (newBottomRightX < bounds.width())
      {
        bottomRight.setX(newBottomRightX);
      }
      else
      {
        bottomRight.setX(bounds.width() - 1);
        actualWidth = bottomRight.x() - topLeft.x();
        bottomRight.setY(topLeft.y() + actualWidth / targetAspectRatio);
      }
    }
  }
}