#ifndef SCHARMELEM_H
#define SCHARMELEM_H

#include <qglobal.h>
#include <QGraphicsItem>
#include <QList>

class SchwarmAlgorithm;
class ElemAudioSource;

enum ScharmElemMode {
    alone, swarm, crowded, annexation, see, collision, farswarm
};

struct CollisionData {
    double distance;
    double time;
};

class PoiElem: public QGraphicsItem
{
public:
    PoiElem(qreal x,qreal y);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    qreal spread() const;
protected:
    void advance(int step) override;
};

class SchwarmElem : public QGraphicsItem, public QObject
{
private:
    qreal vx;
    qreal vy;
    qreal vrotation = 0;
    ScharmElemMode mode = alone;
    SchwarmAlgorithm& schwarmAlgorithm;
    QList<PoiElem*> poiElems;

public:
    SchwarmElem(qreal x,qreal y,qreal vx,qreal vy,SchwarmAlgorithm& schwarmAlgorithm);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    CollisionData computeCollisionDistance(const SchwarmElem &schwarmElem);
    qreal spread() const;
    qreal speed() const;
    void setSpeed(qreal speed);
    void addPoi(PoiElem *poiElem);
    friend class SchwarmAlgorithm;
    friend class BaseSchwarmAlgorithm;
    friend class CurvingSchwarmAlgorithm;
    friend class InteractSchwarmAlgorithm;
    friend class SwarmScene;
    friend class ElemAudioSource;
protected:
    void advance(int step) override;
};


class BarrierElem: public QGraphicsItem
{
public:
    BarrierElem(qreal x,qreal y);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    qreal spread() const;
};



struct CollisionSchwarmElem {
   SchwarmElem *schwarmElem;
   CollisionData collisionData;
};

#endif // SCHARMELEM_H
