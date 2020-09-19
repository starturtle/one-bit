#pragma once
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>

class SourceImage : public QQuickPaintedItem
{
  Q_OBJECT
    Q_PROPERTY(QImage imageBuffer READ data NOTIFY dataChanged)
    Q_PROPERTY(QUrl path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int resultWidth WRITE setResultWidth NOTIFY widthChanged)
    Q_PROPERTY(int resultHeight WRITE setResultHeight NOTIFY heightChanged)
    Q_PROPERTY(int clipX READ clipX)
    Q_PROPERTY(int clipY READ clipY)
    Q_PROPERTY(int clipWidth READ clipWidth)
    Q_PROPERTY(int clipHeight READ clipHeight)
public:
  SourceImage(QQuickItem* parent = nullptr);
  Q_INVOKABLE void setPath(const QUrl& path);
  Q_INVOKABLE void setResultWidth(const int& width);
  Q_INVOKABLE void setResultHeight(const int& height);
  void paint(QPainter* painter);
  QImage data() const;
  int clipWidth() const;
  int clipHeight() const;
  int clipX() const;
  int clipY() const;
  
signals:
  void dataChanged();
  void pathChanged();
  void widthChanged();
  void heightChanged();

protected:
  void mousePressEvent(QMouseEvent* theEvent) final;
  void mouseMoveEvent(QMouseEvent* theEvent) final;
  void mouseReleaseEvent(QMouseEvent* theEvent) final;

private:
  void normalizeLocations();

  QUrl filePath;
  QPoint resultSize;
  QPoint clipTopLeft;
  QPoint clipBottomRight;
  QImage image;
  QPointF topLeft;
  QPointF bottomRight;
  QPointF newTopLeft;
  QPointF newBottomRight;
  bool abort;
  const double aspectRatioMaxDelta = 0.05;
};
