#ifndef SWARMSCENE_H
#define SWARMSCENE_H

#include <QGraphicsScene>
#include "schwarmalgorithm.h"

//! [0]
class SwarmScene : public QGraphicsScene
{
    Q_OBJECT

public:
    void addSwarmItem();

    InteractSchwarmAlgorithm schwarmAlgorithm = InteractSchwarmAlgorithm(0.1,0.05);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    QPointF mousePressPosition;
    bool hasPressed = false;

};

#endif // SWARMSCENE_H
