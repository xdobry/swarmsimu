#ifndef SCHARMELEM_H
#define SCHARMELEM_H

#include <qglobal.h>
#include <QGraphicsItem>

class SchwarmAlgorithm;

enum ScharmElemMode {
    alone, swarm, crowded, annexation, see, collision, farswarm
};

struct CollisionData {
    double distance;
    double time;
};

class SchwarmElem : public QGraphicsItem
{
private:
    qreal vx;
    qreal vy;
    qreal vrotation = 0;
    ScharmElemMode mode = alone;
    SchwarmAlgorithm& schwarmAlgorithm;

public:
    SchwarmElem(qreal x,qreal y,qreal vx,qreal vy,SchwarmAlgorithm& schwarmAlgorithm);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    CollisionData computeCollisionDistance(const SchwarmElem &schwarmElem);
    qreal spread() const;
    friend class SchwarmAlgorithm;
    friend class BaseSchwarmAlgorithm;
    friend class CurvingSchwarmAlgorithm;
    friend class InteractSchwarmAlgorithm;
protected:
    void advance(int step) override;
};

struct CollisionSchwarmElem {
   SchwarmElem *schwarmElem;
   CollisionData collisionData;
};

#endif // SCHARMELEM_H
