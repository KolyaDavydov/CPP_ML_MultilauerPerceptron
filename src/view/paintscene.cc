#include "paintscene.h"

paintScene::paintScene(QObject *parent) : QGraphicsScene(parent) {}

paintScene::~paintScene() {}

void paintScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  addEllipse(event->scenePos().x() - 20, event->scenePos().y() - 20, 40, 40,
             QPen(Qt::NoPen), QBrush(Qt::white));
  previousPoint = event->scenePos();
}

void paintScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  addLine(previousPoint.x(), previousPoint.y(), event->scenePos().x(),
          event->scenePos().y(),
          QPen(Qt::white, 40, Qt::SolidLine, Qt::RoundCap));
  previousPoint = event->scenePos();
}
