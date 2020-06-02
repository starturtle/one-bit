#pragma once
#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QImage>

class ResultImage : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(QImage data READ data WRITE setData NOTIFY dataChanged)
public:
  ResultImage(QQuickItem* parent = nullptr);
  Q_INVOKABLE void setData(const QImage& data);
  void paint(QPainter* painter);
  QImage data() const;
signals:
  void dataChanged();
private:
  QImage resultImage;
};
