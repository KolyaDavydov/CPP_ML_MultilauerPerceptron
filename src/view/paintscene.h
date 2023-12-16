#ifndef CPP7_MLP_1_SRC_VIEW_PAINTSCENE_H
#define CPP7_MLP_1_SRC_VIEW_PAINTSCENE_H

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

class paintScene : public QGraphicsScene {
  Q_OBJECT

 public:
  explicit paintScene(QObject *parent = 0);
  ~paintScene();

 private:
  QPointF previousPoint;

 private:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif  // CPP7_MLP_1_SRC_VIEW_PAINTSCENE_H
