#include "SourceImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>
#include <QPixmap>

namespace
{
  qreal boundedMin(qreal val1, qreal val2, qreal boundary);
  qreal boundedMax(qreal val1, qreal val2, qreal boundary);
  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, const double targetAspectRatio, const qreal paintedWidth, const qreal paintedHeight);
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
  logging::logger().setLogLevel(logging::Level::NOTE);
  setAcceptedMouseButtons(Qt::AllButtons);
}

void SourceImage::mousePressEvent(QMouseEvent* theEvent)
{
  newStartingPoint = { theEvent->localPos().x(), theEvent->localPos().y() };
  logging::logger() << logging::Level::DEBUG << "MousePress: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << logging::Level::OFF;
}

void SourceImage::mouseMoveEvent(QMouseEvent* theEvent)
{
  if (theEvent->buttons() & Qt::MouseButton::LeftButton)
  {
    QRectF bounds = boundingRect();
    logging::logger() << logging::Level::DEBUG << "MouseMove: boundingRect " << bounds.width() << ", " << bounds.height() << logging::Level::OFF;
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height() - 1) };
    newTopLeft = tl;
    newBottomRight = br;
    abort = (theEvent->buttons() & Qt::MouseButton::RightButton);
    if (! abort)
    {
      logging::logger() << logging::Level::DEBUG << "MouseMove: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << ", bottom right is " << newBottomRight.x() << ", " << newBottomRight.y() << logging::Level::OFF;
      update();
    }
  }
}

void SourceImage::mouseReleaseEvent(QMouseEvent* theEvent)
{
  if (abort)
  {
    logging::logger() << logging::Level::NOTE << "MouseRelease (aborted)" << logging::Level::OFF;
  }
  else
  {
    QRectF bounds = boundingRect();
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height()) };
    topLeft = tl;
    bottomRight = br;

    logging::logger() << logging::Level::NOTE << "MouseRelease: top left is " << topLeft.x() << ", " << topLeft.y() << ", bottom right is " << bottomRight.x() << ", " << bottomRight.y() << logging::Level::OFF;
  }
  newStartingPoint = { -1, -1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  update(); // triggers paint(...)
  logging::logger() << logging::Level::NOTE << "Clipping to (" << clipTopLeft.x() << ", " << clipTopLeft.y() << ")/(" << clipBottomRight.x() << ", " << clipBottomRight.y() << ")" << logging::Level::OFF;
  logging::logger() << logging::Level::NOTE << "Based on (" << topLeft.x() << ", " << topLeft.y() << ")/(" << bottomRight.x() << ", " << bottomRight.y() << ")" << logging::Level::OFF;
}

void SourceImage::setPath(const QUrl& data)
{
  filePath = data;
  image.load(filePath.toLocalFile());
  const std::string fileQuality{ image.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::NOTE << "Loaded a new " << fileQuality << "file " << data.toString().toStdString() << logging::Level::OFF;
  topLeft = { 0, 0 };
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  QRectF bounds{ 0, 0, 400, 400 };
#else
  QRectF bounds = boundingRect();
#endif
  bottomRight = { bounds.width() - 1, bounds.height() - 1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  clipTopLeft = { 0, 0 };
  clipBottomRight = { image.width() - 1, image.height() - 1 };
  logging::logger() << logging::Level::NOTE << "File Size is " << image.width() << "x" << image.height() << logging::Level::OFF;
  update(); // triggers paint(...)
}

void SourceImage::setResultWidth(const int& width)
{
  logging::logger() << logging::Level::NOTE << "new width: " << width << logging::Level::OFF;
  resultSize.setX(width);
  update(); // triggers paint(...)
}

void SourceImage::setResultHeight(const int& height)
{
  logging::logger() << logging::Level::NOTE << "new height: " << height << logging::Level::OFF;
  resultSize.setY(height);
  update(); // triggers paint(...)
}

void SourceImage::paint(QPainter* painter) {
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  QRectF bounds{ 0, 0, 400, 400 };
#else
  QRectF bounds = boundingRect();
#endif
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
#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
  QRectF bounds{ 0, 0, 400, 400 };
#else
  QRectF bounds = boundingRect();
#endif
  QImage scaled = image.scaledToWidth(bounds.width());
  logging::logger() << logging::Level::DEBUG << "BoundingBox (" << bounds.x() << ", " << bounds.y() << ")/(" << bounds.width() << ", " << bounds.height() << ")" << logging::Level::OFF;
  if (scaled.height() > bounds.height())
  {
    scaled = image.scaledToHeight(bounds.height());
  }
  const std::string scaledIsValid{ scaled.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::DEBUG << "Rescaled to " << scaledIsValid << "file with " << scaled.width() << "/" << scaled.height() << " pixels" << logging::Level::OFF;
  auto returnValue{ scaled.copy(QRectF(topLeft.x(), topLeft.y(), std::min(bottomRight.x() + 1, (qreal)scaled.width()), std::min(bottomRight.y() + 1, (qreal)scaled.height())).toRect()) };
  const std::string outputIsValid{ returnValue.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::DEBUG << "Actually clipped to " << outputIsValid << "file with " << returnValue.width() << "/" << returnValue.height() << " pixels" << logging::Level::OFF;
  logging::logger() << logging::Level::DEBUG << "Clipping to (" << clipTopLeft.x() << ", " << clipTopLeft.y() << ")/(" << clipBottomRight.x() + 1 << ", " << clipBottomRight.y() + 1 << ")" << logging::Level::OFF;
  const std::string inputIsValid{ image.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::DEBUG << "Returning " << outputIsValid << "file copied from " << inputIsValid << "input" << logging::Level::OFF;
  return returnValue;
}

int SourceImage::clipWidth() const
{
  return clipBottomRight.x() - clipTopLeft.x() + 1;
}

int SourceImage::clipHeight() const
{
  return clipBottomRight.y() - clipTopLeft.y() + 1;
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

  void adjustToAspectRatio(const QPointF& topLeft, QPointF& bottomRight, const double targetAspectRatio, const qreal paintedWidth, const qreal paintedHeight)
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
    return QString("Use " + QString::number(topLeft.x()) + ", " + QString::number(topLeft.y()) + " (w:" + QString::number(bottomRight.x() - topLeft.x() + 1) + ", h:" + QString::number(bottomRight.y() - topLeft.y() + 1) + ")");
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

void testFileLoad(const QUrl& in_url, SourceImage& inout_imgObject, const QString& in_expected_clipping_info, const QSize& in_expected_size)
{
  std::cout << "As url " << in_url.url().toStdString() << std::endl;
  inout_imgObject.setPath(in_url);
  std::cout << inout_imgObject.clippingInfo().toStdString() << std::endl;
  REQUIRE_FALSE(inout_imgObject.data().isNull());
  std::cout << inout_imgObject.clippingInfo().toStdString() << std::endl;
  CHECK_EQ(inout_imgObject.clippingInfo(), in_expected_clipping_info);
  auto actualSize = inout_imgObject.data().size();
  CHECK_EQ(actualSize.width(), in_expected_size.width());
  CHECK_EQ(actualSize.height(), in_expected_size.height());

}

TEST_CASE("test initial image load") {
  SourceImage testImage;
  logging::logger().setLogLevel(logging::Level::DEBUG);
  QUrl resourceUrl;
  resourceUrl.setScheme("file");
  QSize expectedSize;

  resourceUrl.setPath(TEST_FILE_NAME);  
  expectedSize.setWidth(400);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:927, h:927)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_2);
  expectedSize.setWidth(315);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:390, h:496)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_3);
  expectedSize.setWidth(400);
  expectedSize.setHeight(225);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:960, h:540)"), expectedSize);

  resourceUrl.setPath(TEST_FILE_NAME_4);
  expectedSize.setWidth(288);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:708, h:984)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_5);
  expectedSize.setWidth(400);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:400, h:400)"), expectedSize);
}

#endif
