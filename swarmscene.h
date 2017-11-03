#ifndef SWARMSCENE_H
#define SWARMSCENE_H

#include <QGraphicsScene>
#include "schwarmalgorithm.h"
#include <swarmsound.h>

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
    SwarmSound *swarmSound = NULL;
    void initSound(QObject *parent);
    void startSound();
    void stopSound();
    void clearSwarm();
    void suspend();
    void resume();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;


private:
    void addSwarmElem(SchwarmElem *schwarmElem);

    QPointF mousePressPosition;
    bool hasPressed = false;
    ScharmElemAddType schwarmElemAddType;
};

#endif // SWARMSCENE_H
