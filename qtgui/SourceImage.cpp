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

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define GET_BOUNDING_RECTANGLE(name) QRectF name{ 0, 0, 400, 400 }
#else
#define GET_BOUNDING_RECTANGLE(name) QRectF name = boundingRect()
#endif

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define UPDATE_IMAGE() paint(nullptr)
#else
#define UPDATE_IMAGE() update()
#endif

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
  logging::logger() << logging::Level::DEBUG << "MousePress: top left is " << newStartingPoint.x() << ", " << newStartingPoint.y() << logging::Level::OFF;
}

void SourceImage::mouseMoveEvent(QMouseEvent* theEvent)
{
  if (theEvent->buttons() & Qt::MouseButton::LeftButton)
  {
    GET_BOUNDING_RECTANGLE(bounds);
    logging::logger() << logging::Level::DEBUG << "MouseMove: boundingRect " << bounds.width() << ", " << bounds.height() << logging::Level::OFF;
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height() - 1) };
    newTopLeft = tl;
    newBottomRight = br;
    abort = (theEvent->buttons() & Qt::MouseButton::RightButton);
    if (! abort)
    {
      logging::logger() << logging::Level::DEBUG << "MouseMove: top left is " << newTopLeft.x() << ", " << newTopLeft.y() << ", bottom right is " << newBottomRight.x() << ", " << newBottomRight.y() << logging::Level::OFF;
      UPDATE_IMAGE();
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
    GET_BOUNDING_RECTANGLE(bounds);
    QPointF tl = { boundedMin(newStartingPoint.x(), theEvent->localPos().x(), 0), boundedMin(newStartingPoint.y(), theEvent->localPos().y(), 0) };
    QPointF br = { boundedMax(newStartingPoint.x(), theEvent->localPos().x(), bounds.width() - 1), boundedMax(newStartingPoint.y(), theEvent->localPos().y(), bounds.height()) };
    topLeft = tl;
    bottomRight = br;

    logging::logger() << logging::Level::NOTE << "MouseRelease: top left is " << topLeft.x() << ", " << topLeft.y() << ", bottom right is " << bottomRight.x() << ", " << bottomRight.y() << logging::Level::OFF;
  }
  newStartingPoint = { -1, -1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  UPDATE_IMAGE();
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
  GET_BOUNDING_RECTANGLE(bounds);
  bottomRight = { bounds.width() - 1, bounds.height() - 1 };
  newTopLeft = { -1, -1 };
  newBottomRight = { -1, -1 };
  clipTopLeft = { 0, 0 };
  clipBottomRight = { image.width() - 1, image.height() - 1 };
  logging::logger() << logging::Level::NOTE << "File Size is " << image.width() << "x" << image.height() << logging::Level::OFF;
  UPDATE_IMAGE();
}

void SourceImage::setResultWidth(const int& width)
{
  logging::logger() << logging::Level::NOTE << "new width: " << width << logging::Level::OFF;
  resultSize.setX(width);
  UPDATE_IMAGE();
}

void SourceImage::setResultHeight(const int& height)
{
  logging::logger() << logging::Level::NOTE << "new height: " << height << logging::Level::OFF;
  resultSize.setY(height);
  update(); // triggers paint(...)
}

void SourceImage::paint(QPainter* painter) {
  GET_BOUNDING_RECTANGLE(bounds);
  if (image.isNull())
  {
    if (nullptr != painter) painter->fillRect(bounds, Qt::white);
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
  if (nullptr != painter) painter->drawImage(center, scaled);

  normalizeLocations(scaled.width(), scaled.height());
  if (nullptr != painter)
  {
    painter->setPen(QColor(255, 0, 0));
    painter->drawRect(topLeft.x(), topLeft.y(), bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y());
    painter->setPen(QColor(0, 255, 0));
    painter->drawRect(newTopLeft.x(), newTopLeft.y(), newBottomRight.x() - newTopLeft.x(), newBottomRight.y() - newTopLeft.y());
  }
  newClipping();
  dataChanged();
}

QImage SourceImage::data() const
{
  GET_BOUNDING_RECTANGLE(bounds);
  logging::logger() << logging::Level::DEBUG << "BoundingBox (" << bounds.x() << ", " << bounds.y() << ")/(" << bounds.width() << ", " << bounds.height() << ")" << logging::Level::OFF;
  const std::string inputIsValid{ image.isNull() ? "empty " : "" };

  // scale overall image to bounding box
  QImage scaled = image.scaledToWidth(bounds.width());
  if (scaled.height() > bounds.height())
  {
    scaled = image.scaledToHeight(bounds.height());
  }
  const std::string scaledIsValid{ scaled.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::DEBUG << "Rescaled to " << scaledIsValid << "file with " << scaled.width() << "/" << scaled.height() << " pixels" << logging::Level::OFF;

  // clip to selected area
  auto returnValue{ scaled.copy(QRectF(topLeft.x(), topLeft.y(), std::min(1 + bottomRight.x() - topLeft.x(), (qreal)scaled.width()), std::min(1 + bottomRight.y() - topLeft.y(), (qreal)scaled.height())).toRect()) };
  const std::string outputIsValid{ returnValue.isNull() ? "empty " : "" };
  logging::logger() << logging::Level::DEBUG << "Actually clipped to " << outputIsValid << "file with " << returnValue.width() << "/" << returnValue.height() << " pixels" << logging::Level::OFF;

  // validate and return
  logging::logger() << logging::Level::DEBUG << "Clipping to (" << clipX() << ", " << clipY() << ")/(" << clipWidth() << ", " << clipHeight() << ")" << logging::Level::OFF;
  logging::logger() << logging::Level::DEBUG << "Returning " << outputIsValid << "file copied from " << inputIsValid << "input" << logging::Level::OFF;
  return returnValue;
}

int SourceImage::clipWidth() const
{
  return 1 + clipBottomRight.x() - clipTopLeft.x();
}

int SourceImage::clipHeight() const
{
  return 1 + clipBottomRight.y() - clipTopLeft.y();
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
  logging::logger() << logging::Level::DEBUG << "Normalizing..." << logging::Level::OFF;
  double definedAspectRatio = (resultSize.x() > 0 && resultSize.y() > 0) ? (1. * resultSize.y() / resultSize.x()) : (1. * image.height() / image.width());

  adjustToAspectRatio(topLeft, bottomRight, definedAspectRatio, paintedWidth, paintedHeight);
  adjustToAspectRatio(newTopLeft, newBottomRight, definedAspectRatio, paintedWidth, paintedHeight);

  qreal scaling{ image.width() / paintedWidth };
  logging::logger() << logging::Level::DEBUG << "Using scaling between displayed and actual image of " << scaling << " from " << image.width() << "x" << image.height() << " to " << paintedWidth << "x" << paintedHeight << logging::Level::OFF;
  scalePoint(topLeft, clipTopLeft, scaling);
  scalePoint(bottomRight, clipBottomRight, scaling);
  scalePoint(newTopLeft, newClipTopLeft, scaling);
  scalePoint(newBottomRight, newClipBottomRight, scaling);
  logging::logger() << logging::Level::DEBUG << "Done." << logging::Level::OFF;
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
    logging::logger() << logging::Level::DEBUG << "Adjusting " << topLeft.x() << "/"<< topLeft.y() << " and " << bottomRight.x() << "/" << bottomRight.y() << " to aspect ratio " << targetAspectRatio << " on canvas " << paintedWidth << "x" << paintedHeight << logging::Level::OFF;
    if (bottomRight.x() >= paintedWidth)
      bottomRight.setX(paintedWidth - 1);
    if (bottomRight.y() >= paintedHeight)
      bottomRight.setY(paintedHeight - 1);

    double actualHeight = 1 + bottomRight.y() - topLeft.y();
    double actualWidth = 1 + bottomRight.x() - topLeft.x();
    double actualAspectRatio = actualHeight / actualWidth;
    static const double aspectRatioMaxDelta{ 0.02 };

    if (std::abs(targetAspectRatio - actualAspectRatio) < aspectRatioMaxDelta)
    {
      return;
    }

    // adjust the smaller one
    if (actualWidth > actualHeight)
    {
      qreal newBottomRightY{ topLeft.y() - 1 + actualWidth * targetAspectRatio };
      if (newBottomRightY < paintedHeight)
      {
        bottomRight.setY(newBottomRightY);
      }
      else
      {
        bottomRight.setY(paintedHeight - 1);
        actualHeight = 1 + bottomRight.y() - topLeft.y();
        bottomRight.setX(topLeft.x() - 1 + actualHeight / targetAspectRatio );
      }
    }
    else if (actualWidth < actualHeight)
    {
      qreal newBottomRightX{ topLeft.x() - 1 + actualHeight / targetAspectRatio };
      if (newBottomRightX < paintedWidth)
      {
        bottomRight.setX(newBottomRightX);
      }
      else
      {
        bottomRight.setX(paintedWidth - 1);
        actualWidth = 1 + bottomRight.x() - topLeft.x();
        bottomRight.setY(topLeft.y() - 1 + actualWidth / targetAspectRatio);
      }
    }
    else if (targetAspectRatio > 1.)
    {
      // y > x, adjust x.
      qreal newBottomRightX{ topLeft.x() - 1 + actualHeight / targetAspectRatio };
      bottomRight.setX(newBottomRightX);
    }
    else
    {
      qreal newBottomRightY{ topLeft.y() - 1 + actualWidth * targetAspectRatio };
      bottomRight.setY(newBottomRightY);
    }
    logging::logger() << logging::Level::DEBUG << "Adjusted " << topLeft.x() << "/" << topLeft.y() << " and " << bottomRight.x() << "/" << bottomRight.y() << " to aspect ratio " << targetAspectRatio << " on canvas " << actualWidth << "x" << actualHeight << logging::Level::OFF;
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

class SourceImageTester
{
public:
  SourceImageTester(SourceImage& imageToTest) : m_imageToTest{ imageToTest } {}
  
  inline void mouseDown(const QPointF& pos)
  {
    QMouseEvent mousePressed(QEvent::Type::MouseButtonPress, pos, Qt::MouseButton::LeftButton, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
    m_imageToTest.mousePressEvent(&mousePressed);
  }
  
  inline void mouseMoved(const QPointF& pos)
  {
    QMouseEvent mouseMoved(QEvent::Type::MouseMove, pos, Qt::MouseButton::NoButton, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier);
    m_imageToTest.mouseMoveEvent(&mouseMoved);
  }
  
  inline void mouseUp(const QPointF& pos, bool canceled = false)
  {
    QMouseEvent mouseReleased(QEvent::Type::MouseButtonRelease, pos, Qt::MouseButton::LeftButton, (canceled ? Qt::MouseButton::RightButton : Qt::MouseButton::NoButton), Qt::KeyboardModifier::NoModifier);
    m_imageToTest.mouseReleaseEvent(&mouseReleased);
  }

  inline QImage& fullImage() { return m_imageToTest.image; }
private:
  SourceImage& m_imageToTest;
};

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
  const qreal maxDelta{ 1. };

  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 249.) < maxDelta);
  CHECK(std::abs(bottomRight.y() - 249.) < maxDelta);

  targetAspectRatio = .8;
  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 249.) < maxDelta);
  CHECK(std::abs(bottomRight.y() - 199.2) < maxDelta);

  targetAspectRatio = 2.;
  adjustToAspectRatio(topLeft, bottomRight, targetAspectRatio, paintedWidth, paintedHeight);
  CHECK(std::abs(bottomRight.x() - 124.5) < maxDelta);
  CHECK(std::abs(bottomRight.y() - 249.) < maxDelta);
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

void fileSizeCheck(const SourceImage& inout_imgObject, const QString& in_expected_clipping_info, const QSize& in_expected_size)
{
  REQUIRE_FALSE(inout_imgObject.data().isNull());
  std::cout << inout_imgObject.clippingInfo().toStdString() << std::endl;
  CHECK_EQ(inout_imgObject.clippingInfo(), in_expected_clipping_info);
  auto actualSize = inout_imgObject.data().size();
  CHECK_EQ(actualSize.width(), in_expected_size.width());
  CHECK_EQ(actualSize.height(), in_expected_size.height());
}

void testFileLoad(const QUrl& in_url, SourceImage& inout_imgObject, const QString& in_expected_clipping_info, const QSize& in_expected_size)
{
  std::cout << "As url " << in_url.url().toStdString() << std::endl;
  inout_imgObject.setPath(in_url);
  std::cout << inout_imgObject.clippingInfo().toStdString() << std::endl;
  fileSizeCheck(inout_imgObject, in_expected_clipping_info, in_expected_size);
}

TEST_CASE("test initial image load") {
  SourceImage testImage;
  logging::logger().setLogLevel(logging::Level::DEBUG);
  QUrl resourceUrl;
  resourceUrl.setScheme("file");
  QSize expectedSize;
  // NOTE: most image sizes are actually slightly larger, but the clipping settings are performed on the downsized image file, giving an accuracy of about 2.x pixels on the original image per displayed pixel
  resourceUrl.setPath(TEST_FILE_NAME);  
  expectedSize.setWidth(400);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:925, h:925)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_2);
  expectedSize.setWidth(315);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:389, h:495)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_3);
  expectedSize.setWidth(400);
  expectedSize.setHeight(225);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:958, h:538)"), expectedSize);

  resourceUrl.setPath(TEST_FILE_NAME_4);
  expectedSize.setWidth(288);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:706, h:981)"), expectedSize);
  
  resourceUrl.setPath(TEST_FILE_NAME_5);
  expectedSize.setWidth(400);
  expectedSize.setHeight(400);
  testFileLoad(resourceUrl, testImage, QString("Use 0, 0 (w:400, h:400)"), expectedSize);
}

const std::string qrgbToHexString(const QRgb& in_color)
{
  std::stringstream formatted{};
  formatted << "#" << std::setfill('0') << std::hex;
  for (auto i = 0; i < 4; ++i) {
    formatted << std::setw(2) << (in_color >> 8*(3-i))%256;
  }
  return formatted.str();
}

unsigned compareQRgb(const QRgb& value1, const QRgb& value2, uchar maxDifference = 10)
{
  auto cv1 = value1;
  auto cv2 = value2;
  unsigned rv = 0;
  for (auto i = 0; i < 4; ++i) {
    uchar byte1 = cv1 % 256;
    uchar byte2 = cv2 % 256;
    unsigned diff = std::abs(byte1 - byte2);
    if (diff > rv) rv = diff;
  }
  return rv;
  //return true;
}

struct RangeSelectionTestSet
{
  std::string name;
  QPoint topLeft;
  QPoint bottomRight;
  QPoint topLeftOriginal;
  QSize sizeOriginal;
  QSize sizeScaled() const { return QSize{ bottomRight.x() - topLeft.x() + 1, bottomRight.y() - topLeft.y() + 1 }; }
  QString toClippingString() const {
    return QString("Use " + QString::number(topLeftOriginal.x()) + ", " + QString::number(topLeftOriginal.y()) + " (w:" + QString::number(sizeOriginal.width()) + ", h:" + QString::number(sizeOriginal.height()) + ")");
  }
};

void compareSourceImageData(SourceImage& testImage, SourceImageTester& tester, const RangeSelectionTestSet& testArgs)
{
  auto testCroppedSection = testImage.data();
  auto testRectangle = tester.fullImage().copy({ testArgs.topLeftOriginal, testArgs.sizeOriginal }).scaledToWidth(testArgs.bottomRight.x() - testArgs.topLeft.x() + 1);
  CHECK_EQ(testCroppedSection.width(), testRectangle.width());
  CHECK_EQ(testCroppedSection.height(), testRectangle.height());
  std::cout << "The comparison isn't 100% accurate around edges. Compare manually!" << std::endl;
  std::cout << "The following gives an ASCII art representation of the diff between the two image sections by pixel channel values." << std::endl;
  unsigned criticalCount = 0;
  for (int y = 0; y < testCroppedSection.height(); y++) {
    QRgb* lineToTest = (QRgb*)testCroppedSection.scanLine(y);
    QRgb* lineToMatch = (QRgb*)testRectangle.scanLine(y);
    std::cout << "|";
    for (int x = 0; x < testCroppedSection.width(); x++) {
      auto frame = compareQRgb(lineToTest[x], lineToMatch[x]);
      char pixel = ' ';
      if (frame >= 32) { pixel = 'X'; ++criticalCount; }
      else if (frame >= 16) pixel = 'x';
      else if (frame >= 8) pixel = '_';
      else if (frame >= 4) pixel = '.';
      std::cout << pixel;
    }
    std::cout << "|" << std::endl;
  }
  auto size = testArgs.sizeScaled();
  CHECK_LT(criticalCount, size.width() * size.height() / 50); // less than 1% actually at high contrast by pixel
}

TEST_CASE("test mouse based range selection")
{
  SourceImage testImage;
  logging::logger().setLogLevel(logging::Level::DEBUG);

  testImage.setResultWidth(10);
  testImage.setResultHeight(10);

  QUrl resourceUrl;
  resourceUrl.setScheme("file");
  resourceUrl.setPath(TEST_FILE_NAME);
  testImage.setPath(resourceUrl);

  SourceImageTester tester{ testImage };
  
  std::vector<RangeSelectionTestSet> movements{
    {
      "wibbel_topleft.jpg",
      {10, 10},
      {100, 100},
      {23, 23},
      {209, 209},
    },{
      "wibbel_middle.jpg",
      {150, 150},
      {300, 300},
      {347, 347},
      {349, 349},
    },{
      "wibbel_bottomright.jpg",
      {250, 250},
      {350, 350},
      {579, 579},
      {233, 233},
    } 
  };
  for (auto& parameters : movements)
  {
    // drag rectangle from 10, 10 to 100, 100 computationally
    tester.mouseDown(parameters.topLeft);
    for (qreal pos = parameters.topLeft.x() + 10; pos < parameters.bottomRight.x(); pos += 10) tester.mouseMoved({ pos, pos });
    tester.mouseUp(parameters.bottomRight);

    // this mouse action should result in a rectangle with 191x191 displayed pixels starting at 10x10 displayed
    // this rectangle, if resized to match the overall image size of 925x925, scales to a 441x441 rectangle starting at pixel 23x23.
    fileSizeCheck(testImage, parameters.toClippingString(), parameters.sizeScaled());
    compareSourceImageData(testImage, tester, parameters);
    QString outPath(std::string(TEST_DIR_NAME + parameters.name).c_str());
    testImage.data().save(outPath);
  }
}

#endif
