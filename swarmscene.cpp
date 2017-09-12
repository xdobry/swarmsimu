#include "swarmscene.h"
#include "schwarmelem.h"

#include <QGraphicsSceneMouseEvent>


void SwarmScene::addSwarmItem()
{
    qreal vmax = 5;
    qreal xmax = 100;
    qreal ymax = 20;
    qreal x = ((qreal)rand()/RAND_MAX)*xmax-xmax/2;
    qreal y = ((qreal)rand()/RAND_MAX)*ymax-ymax/2;
    qreal vx = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
    qreal vy = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
    SchwarmElem *schwarm = new SchwarmElem(x,y,vx,vy,schwarmAlgorithm);
     addItem(schwarm);

    /*
    SchwarmElem *schwarm = new SchwarmElem(-5,0,0.4,0.0,schwarmAlgorithm);
    addItem(schwarm);
    schwarm = new SchwarmElem(-25,0,0.6,0.0,schwarmAlgorithm);
    addItem(schwarm);
    */
}

void SwarmScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        mousePressPosition = mouseEvent->scenePos();
        hasPressed = true;
    }

    QGraphicsScene::mousePressEvent(mouseEvent);
}

void SwarmScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        if (hasPressed) {
            QPointF endMouse = mouseEvent->scenePos();
            qreal vx = (endMouse.x()-mousePressPosition.x())/5;
            qreal vy = (endMouse.y()-mousePressPosition.y())/5;
            SchwarmElem *schwarm = new SchwarmElem(mousePressPosition.x(),mousePressPosition.y(),vx,vy,schwarmAlgorithm);
            addItem(schwarm);
        }
        hasPressed = false;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
