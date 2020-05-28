#pragma once
#include <QObject>
#include <QImage>
#include <QUrl>
#include <QColor>

#include <vector>


class QtPixelator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QImage result READ result)
public:
    explicit QtPixelator (QObject* in_parent = nullptr);
    Q_INVOKABLE int run();
    Q_INVOKABLE int setInputImage(const QUrl& in_url);
    Q_INVOKABLE int setOutputImage(const QUrl& in_url);
    Q_INVOKABLE int setStitchSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge);
    Q_INVOKABLE int setStitchColors(const QColor& in_color1, const QColor& in_color2);

    const QImage& result() const;

private:
    void recomputeSizes(const int& in_width, const int& in_height, const int& in_rowsPerGauge, const int& in_stitchesPerGauge);
    void pixelate();

    QImage before;
    QImage after;
    QUrl storagePath;
    unsigned stitchWidth;
    unsigned stitchHeight;
    unsigned stitchCount;
    unsigned rowCount;
    std::vector<QColor> colors;
};