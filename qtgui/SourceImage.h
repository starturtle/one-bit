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
public:
  SourceImage(QQuickItem* parent = nullptr);
  Q_INVOKABLE void setPath(const QUrl& path);
  void paint(QPainter* painter);
  QImage data() const;
  QUrl path() const;
  
signals:
  void dataChanged();
  void pathChanged();

protected:
  void mousePressEvent(QMouseEvent* theEvent) final;
  void mouseMoveEvent(QMouseEvent* theEvent) final;
  void mouseReleaseEvent(QMouseEvent* theEvent) final;

private:
  QUrl filePath;
  QImage image;
  QPointF topLeft;
  QPointF bottomRight;
  QPointF newTopLeft;
  QPointF newBottomRight;
  bool abort;
};
