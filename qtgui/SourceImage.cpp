#include "SourceImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

namespace
{
  qreal boundedMin(qreal val1, qreal val2, qreal boundary);
  qreal boundedMax(qreal val1, qreal val2, qreal boundary);
  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, double targetAspectRatio, qreal paintedWidth, qreal paintedHeight);
  QString pointsToClippingInfo(const QPointF& topLeft, const QPointF& bottomRight);
  void scalePoint(const QPointF& source, QPoint& target, qreal scalingFactor);
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
  logging::LogStream::instance().setLogLevel(logging::Level::INFO);
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
    logging::LogStream::instance().getLogStream(logging::Level::INFO) << "MouseRelease (aborted)" << std::endl;
  }
  else
  {
    QRectF bounds = boundingRect();
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height()) };
    topLeft = tl;
    bottomRight = br;

    logging::LogStream::instance().getLogStream(logging::Level::INFO) << "MouseRelease: top left is " << topLeft.x() << ", " << topLeft.y() << ", bottom right is " << bottomRight.x() << ", " << bottomRight.y() << std::endl;
  }
  newStartingPoint = { -1, -1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  update(); // triggers paint(...)
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "Clipping to (" << clipTopLeft.x() << ", " << clipTopLeft.y() << ")/(" << clipBottomRight.x() << ", " << clipBottomRight.y() << ")" << std::endl;
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "Based on (" << topLeft.x() << ", " << topLeft.y() << ")/(" << bottomRight.x() << ", " << bottomRight.y() << ")" << std::endl;
}

void SourceImage::setPath(const QUrl& data)
{
  filePath = data;
  image.load(data.toLocalFile());
  const std::string fileQuality{ image.isNull() ? "empty " : "" };
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "Loaded a new " << fileQuality << "file" << std::endl;
  topLeft = { 0, 0 };
  QRectF bounds = boundingRect();
  bottomRight = { bounds.width() - 1, bounds.height() - 1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  clipTopLeft = { 0, 0 };
  clipBottomRight = { image.width() - 1, image.height() - 1 };
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "File Size is " << image.width() << "x" << image.height() << std::endl;
  update(); // triggers paint(...)
}

void SourceImage::setResultWidth(const int& width)
{
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "new width: " << width << std::endl;
  resultSize.setX(width);
  update(); // triggers paint(...)
}

void SourceImage::setResultHeight(const int& height)
{
  logging::LogStream::instance().getLogStream(logging::Level::INFO) << "new height: " << height << std::endl;
  resultSize.setY(height);
  update(); // triggers paint(...)
}

void SourceImage::paint(QPainter* painter) {
  QRectF bounds = boundingRect();
  if (image.isNull())
  {
    painter->fillRect(bounds, Qt::white);
    return;
  }
  QImage scaled = image.scaledToWidth(bounds.width());
  if (scaled.height() > bounds.height())
  {
    scaled = image.scaledToHeight(bounds.height());
  }
  QPointF center = bounds.center() - scaled.rect().center();

  if (center.x() < 0)
    center.setX(0);
  if (center.y() < 0)
    center.setY(0);
  setHeight(scaled.height());
  setWidth(scaled.width());
  painter->drawImage(center, scaled);
  normalizeLocations(scaled.width(), scaled.height());
  painter->setPen(QColor(255, 0, 0));
  painter->drawRect(topLeft.x(), topLeft.y(), bottomRight.x()-topLeft.x(), bottomRight.y()-topLeft.y());
  painter->setPen(QColor(0, 255, 0));
  painter->drawRect(newTopLeft.x(), newTopLeft.y(), newBottomRight.x() - newTopLeft.x(), newBottomRight.y() - newTopLeft.y());

  newClipping();
  dataChanged();
}

QImage SourceImage::data() const
{
  QRectF bounds = boundingRect();
  QImage scaled = image.scaledToWidth(bounds.width());
  if (scaled.height() > bounds.height())
  {
    scaled = image.scaledToHeight(bounds.height());
  }
  auto returnValue{ scaled.copy(QRectF(topLeft, bottomRight).toRect()) };
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

QString SourceImage::clippingInfo() const
{
  if (newStartingPoint.x() >= 0)
  {
    return pointsToClippingInfo(newClipTopLeft, newClipBottomRight);
  }
  return pointsToClippingInfo(clipTopLeft, clipBottomRight);
}

void SourceImage::normalizeLocations(qreal paintedWidth, qreal paintedHeight)
{
  double definedAspectRatio = resultSize.y() / resultSize.x();

  adjustToAspectRatio(topLeft, bottomRight, definedAspectRatio, paintedWidth, paintedHeight);
  adjustToAspectRatio(newTopLeft, newBottomRight, definedAspectRatio, paintedWidth, paintedHeight);

  qreal scaling{ image.width() / paintedWidth };
  scalePoint(topLeft, clipTopLeft, scaling);
  scalePoint(bottomRight, clipBottomRight, scaling);
  scalePoint(newTopLeft, newClipTopLeft, scaling);
  scalePoint(newBottomRight, newClipBottomRight, scaling);
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

  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, double targetAspectRatio, qreal paintedWidth, qreal paintedHeight)
  {
    if (bottomRight.x() >= paintedWidth)
      bottomRight.setX(paintedWidth - 1);
    if (bottomRight.y() >= paintedHeight)
      bottomRight.setY(paintedHeight - 1);
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
      if (newBottomRightY < paintedHeight)
      {
        bottomRight.setY(newBottomRightY);
      }
      else
      {
        bottomRight.setY(paintedHeight - 1);
        actualHeight = bottomRight.y() - topLeft.y();
        bottomRight.setX(topLeft.x() + actualHeight / targetAspectRatio);
      }
    }
    else if (actualWidth < actualHeight)
    {
      qreal newBottomRightX{ topLeft.x() + actualHeight / targetAspectRatio };
      if (newBottomRightX < paintedWidth)
      {
        bottomRight.setX(newBottomRightX);
      }
      else
      {
        bottomRight.setX(paintedWidth - 1);
        actualWidth = bottomRight.x() - topLeft.x();
        bottomRight.setY(topLeft.y() + actualWidth / targetAspectRatio);
      }
    }
    else if (targetAspectRatio > 1.)
    {
      // y > x, adjust x.
      qreal newBottomRightX{ topLeft.x() + actualHeight / targetAspectRatio };
      bottomRight.setX(newBottomRightX);
    }
    else
    {
      qreal newBottomRightY{ topLeft.y() + actualWidth * targetAspectRatio };
      bottomRight.setY(newBottomRightY);
    }
  }
  
  QString pointsToClippingInfo(const QPointF& topLeft, const QPointF& bottomRight)
  {
    return QString("Use " + QString::number(topLeft.x()) + ", " + QString::number(topLeft.y()) + " (w:" + QString::number(bottomRight.x() - topLeft.x()) + ", h:" + QString::number(bottomRight.y() - topLeft.y()) + ")");
  }

  void scalePoint(const QPointF& source, QPoint& target, qreal scalingFactor)
  {
    target.setX(source.x() * scalingFactor);
    target.setY(source.y() * scalingFactor);
  }
}

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

TEST_CASE("test boundedMin") {
  CHECK_EQ(boundedMin(1, 2, 3), 3);
  CHECK_EQ(boundedMin(3, 2, 1), 2);
}
TEST_CASE("test boundedMax") {
  CHECK_EQ(boundedMax(4, 3, 2), 2);
  CHECK_EQ(boundedMax(2, 3, 4), 3);
}
TEST_CASE("test adjustToAspectRatio") {
  QPointF topLeft{0, 0};
  double targetAspectRatio{ 1. };
  QPointF bottomRight{200, 250};
  qreal paintedWidth{ 250 };
  qreal paintedHeight{ 250 };

  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 249) < 0.1);
  CHECK(std::abs(bottomRight.y() - 249) < 0.1);

  targetAspectRatio = .8;
  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 249) < 0.1);
  CHECK(std::abs(bottomRight.y() - 199.2) < 0.1);

  targetAspectRatio = 2.;
  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 124.5) < 0.1);
  CHECK(std::abs(bottomRight.y() - 249) < 0.1);
}

TEST_CASE("test scalePoint") {
  QPointF pointToScale{ 5., 5. };
  QPoint target;

  scalePoint(pointToScale, target, .5);
  CHECK_EQ(target.x(), 2);
  CHECK_EQ(target.x(), 2);

  scalePoint(pointToScale, target, 4.);
  CHECK_EQ(target.x(), 20);
  CHECK_EQ(target.x(), 20);
}
#endif