#ifndef SWARMSCENE_H
#define SWARMSCENE_H

#include <QGraphicsScene>
#include "schwarmalgorithm.h"

//! [0]
class SwarmScene : public QGraphicsScene
{
    Q_OBJECT   

public:
    enum ScharmElemAddType {
        swarm, poi, barrier
    };
    void addSwarmItem();

    InteractSchwarmAlgorithm schwarmAlgorithm = InteractSchwarmAlgorithm(15,0.05);

    void setAddElemType(ScharmElemAddType schwarmElemAddType);
    ScharmElemAddType getAddElemType();
    void openFile(const QString &fileName);
    bool saveFile(const QString &fileName);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;


private:
    QPointF mousePressPosition;
    bool hasPressed = false;
    ScharmElemAddType schwarmElemAddType;

};

#endif // SWARMSCENE_H
