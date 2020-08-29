#include "SourceImage.h"

#include "logging.h"

#include <algorithm>
#include <QMouseEvent>

SourceImage::SourceImage(QQuickItem* parent)
: QQuickPaintedItem()
, filePath{}
, resultSize{}
, clipTopLeft{}
, clipBottomRight{}
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
    abort = (theEvent->buttons() & Qt::MouseButton::RightButton);
    if (! abort)
    {
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
  const std::string outputQuality{ returnValue.isNull() ? "empty " : "" };
  const std::string inputQuality{ image.isNull() ? "empty " : "" };
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Returning " << outputQuality << "file copied from " << inputQuality << "input" << std::endl;
  const std::string filePathAndName = filePath.toLocalFile().toStdString();
  const std::string ending = filePathAndName.substr(filePathAndName.length() - 4);
  QString savePath = QString::fromStdString(filePathAndName.substr(0, filePathAndName.length() - 4) + "_temp" + ending);
  logging::LogStream::instance().getLogStream(logging::Level::DEBUG) << "Store a copy to " << savePath.toStdString() << std::endl;
  returnValue.save(savePath);
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
  int tempHeight = newBottomRight.y() - newTopLeft.y();
  int tempWidth = newBottomRight.x() - newTopLeft.x();
  double temporaryAspectRatio = (1. * tempHeight) / tempWidth;
  int height = bottomRight.y() - topLeft.y();
  int width = bottomRight.x() - topLeft.x();
  double actualAspectRatio = (1. * height) / width;

  if (std::abs(definedAspectRatio - temporaryAspectRatio) > aspectRatioMaxDelta)
  {
    // adjust the smaller one
    if (tempWidth > tempHeight)
    {
      newBottomRight.setY(newTopLeft.y() + tempWidth * definedAspectRatio);
      tempWidth = newBottomRight.x() - newTopLeft.x();
    }
    else
    {
      newBottomRight.setX(newTopLeft.x() + tempHeight / definedAspectRatio);
      tempHeight = newBottomRight.y() - newTopLeft.y();
    }
  }

  if (std::abs(definedAspectRatio - actualAspectRatio) > aspectRatioMaxDelta)
  {
    // adjust the smaller one
    if (width > height)
    {
      bottomRight.setY(topLeft.y() + width * definedAspectRatio);
      height = bottomRight.y() - topLeft.y();
    }
    else
    {
      bottomRight.setX(topLeft.x() + height / definedAspectRatio);
      width = bottomRight.x() - topLeft.x();
    }
  }

  QRectF bounds = boundingRect();
  clipTopLeft.setX(topLeft.x() * image.width() / bounds.width());
  clipTopLeft.setY(topLeft.y() * image.height() / bounds.height());
  clipBottomRight.setX(bottomRight.x() * image.width() / bounds.width());
  clipBottomRight.setY(bottomRight.y() * image.height() / bounds.height());
}