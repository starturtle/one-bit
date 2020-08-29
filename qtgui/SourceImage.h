#pragma once
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>

class SourceImage : public QQuickPaintedItem
{
  Q_OBJECT
    Q_PROPERTY(QImage data READ data)
    Q_PROPERTY(QUrl path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int clipWidth READ clipWidth WRITE setSelectionWidth NOTIFY widthChanged)
    Q_PROPERTY(int clipHeight READ clipHeight WRITE setSelectionHeight NOTIFY heightChanged)
public:
  SourceImage(QQuickItem* parent = nullptr);
  Q_INVOKABLE void setPath(const QUrl& path);
  Q_INVOKABLE void setSelectionWidth(const int& width);
  Q_INVOKABLE void setSelectionHeight(const int& height);
  void paint(QPainter* painter);
  QImage data() const;
  int clipWidth() const;
  int clipHeight() const;
  
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
  QImage image;
  QPointF topLeft;
  QPointF bottomRight;
  QPointF newTopLeft;
  QPointF newBottomRight;
  bool abort;
  const double aspectRatioMaxDelta = 0.05;
};
