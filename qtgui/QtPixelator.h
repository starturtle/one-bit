#pragma once
#include <QObject>
#include <QImage>
#include <QUrl>
#include <QColor>

#include <vector>


class QtPixelator : public QObject {
  Q_OBJECT
  Q_PROPERTY(QImage resultBuffer READ resultImage)
public:
  explicit QtPixelator (QObject* in_parent = nullptr);
  Q_INVOKABLE int run();
  Q_INVOKABLE int commit();
  Q_INVOKABLE int setInputImage(const QImage& in_image);
  Q_INVOKABLE int setStoragePath(const QUrl& in_url);
  Q_INVOKABLE int setStitchSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge);
  Q_INVOKABLE int setStitchColors(const std::vector<QColor> in_colors);
  Q_INVOKABLE int setHelperSettings(bool gridEnabled, QColor primaryColor, QColor secondaryColor, unsigned gridCount);

  QImage resultImage() const;
signals:
  void pixelationCreated();

private:
  void recomputeSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge);
  QImage pixelate();
  bool scalePixels(const QImage& colorMap);
  void drawHelpers();
  int checkSettings();

  QImage imageBuffer;
  QImage resultBuffer;
  QUrl sourcePath;
  QUrl storagePath;
  unsigned stitchWidth;
  unsigned stitchHeight;
  unsigned stitchCount;
  unsigned rowCount;
  std::vector<QColor> colors;
  QColor auxColorSec;
  QColor auxColorPri;
  unsigned helperGrid;
  bool gridEnabled;
};