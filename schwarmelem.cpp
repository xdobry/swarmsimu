#include "schwarmelem.h"
#include "schwarmalgorithm.h"
#include "vecmath.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#define schwarm_size 6
#define poi_size 12
#define barrier_size 12

SchwarmElem::SchwarmElem(qreal lx, qreal ly, qreal lvx, qreal lvy, SchwarmAlgorithm& sa) : vx(lvx), vy(lvy), schwarmAlgorithm(sa) {
    setPos(lx,ly);
}

void SchwarmElem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    switch (mode) {
    case swarm:
        painter->setBrush(Qt::green);
        break;
    case farswarm:
        painter->setBrush(Qt::darkGreen);
        break;
    case crowded:
        painter->setBrush(Qt::red);
        break;
    case annexation:
        painter->setBrush(Qt::blue);
        break;
    case see:
        painter->setBrush(Qt::yellow);
        break;
    case collision:
        painter->setBrush(Qt::magenta);
        break;
    default:
        painter->setBrush(Qt::black);
        break;
    }
    painter->drawEllipse(QRectF(-schwarm_size/2, -schwarm_size/2, schwarm_size, schwarm_size));
}

CollisionData SchwarmElem::computeCollisionDistance(const SchwarmElem &schwarmElem)
{
    CollisionData collisionData;
    collisionData.distance = -1;
    collisionData.time = 0;
    // check if collision is possible
    // TODO man braucht hier auch luft weil es verdamp knapp sein könnte
    // gerade wenn werte sehr ähnlich
    // Problem wenn die Kurse fast entgegengesetzt sind, dann kann man objekt nicht mehr als Punkt betrachten, wei
    // als punkte keine Kollision möglich ist, dann müsste man spezielle rand punkte berechnen
    if (x()+spread()*3<schwarmElem.x() && vx<=schwarmElem.vx) {
        return collisionData;
    }
    if (x()-spread()*3>schwarmElem.x() && vx>=schwarmElem.vx) {
        return collisionData;
    }
    if (y()+spread()*3<schwarmElem.y() && vy<=schwarmElem.vy) {
        return collisionData;
    }
    if (y()-spread()*3>schwarmElem.y() && vy>=schwarmElem.vy) {
        return collisionData;
    }
    // compute distance in time, use x or y depending of distance
    qreal collisionTime;
    if (qAbs(x()-schwarmElem.x())>qAbs(y()-schwarmElem.y())) {
        collisionTime = qAbs(x()-schwarmElem.x())/qAbs(schwarmElem.vx-vx);
    } else {
        collisionTime = qAbs(y()-schwarmElem.y())/qAbs(schwarmElem.vy-vy);
    }
    collisionData.time = collisionTime;
    qreal x1 = x()+collisionTime*this->vx;
    qreal y1 = y()+collisionTime*this->vy;
    qreal x2 = schwarmElem.x()+collisionTime*schwarmElem.vx;
    qreal y2 = schwarmElem.y()+collisionTime*schwarmElem.vy;
    collisionData.distance = pointDistance(x1,y1,x2,y2);
    return collisionData;
}

qreal SchwarmElem::spread() const
{
    return schwarm_size;
}

qreal SchwarmElem::speed() const
{
    return vectorSpeed(vx,vy);
}

void SchwarmElem::setSpeed(qreal speed)
{
    setVectorLen(vx,vy,speed);
}

void SchwarmElem::addPoi(PoiElem *poiElem)
{
    poiElems.append(poiElem);
}

void SchwarmElem::advance(int step)
{
    if (!step)
        return;
    schwarmAlgorithm.advance(*this);
}

QRectF SchwarmElem::boundingRect() const
{
    return QRectF(-schwarm_size/2, -schwarm_size/2,schwarm_size,schwarm_size);
}

QPainterPath SchwarmElem::shape() const
{
    QPainterPath path;
    path.addRect(-schwarm_size/2, -schwarm_size/2, schwarm_size, schwarm_size);
    return path;
}



PoiElem::PoiElem(qreal x, qreal y)
{
    setPos(x,y);
}

qreal PoiElem::spread() const
{
    return poi_size;
}

void PoiElem::advance(int step)
{
    if (!step) {
        double nextRegion = 80;
        QPolygonF seeFeld;
        seeFeld<<QPointF(-nextRegion,-nextRegion)<<QPointF(-nextRegion,nextRegion)<<QPointF(nextRegion,nextRegion)<<QPointF(nextRegion,-nextRegion);
        QList<QGraphicsItem *> nearItems = scene()->items(mapToScene(seeFeld));
        foreach (QGraphicsItem *item, nearItems) {
            SchwarmElem *nextSchwarmElem = dynamic_cast<SchwarmElem*>(item);
            if (nextSchwarmElem) {
                nextSchwarmElem->addPoi(this);
            }
        }
    }
}

QRectF PoiElem::boundingRect() const
{
    return QRectF(-poi_size/2, -poi_size/2,poi_size,poi_size);
}

QPainterPath PoiElem::shape() const
{
    QPainterPath path;
    path.addRect(-poi_size/2, -poi_size/2, poi_size, poi_size);
    return path;
}

void PoiElem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::green);
    painter->drawEllipse(QRectF(-poi_size/2, -poi_size/2, poi_size, poi_size));
}

BarrierElem::BarrierElem(qreal x, qreal y)
{
    setPos(x,y);
}

qreal BarrierElem::spread() const
{
    return barrier_size;
}

QRectF BarrierElem::boundingRect() const
{
    return QRectF(-barrier_size/2, -barrier_size/2,barrier_size,barrier_size);
}

QPainterPath BarrierElem::shape() const
{
    QPainterPath path;
    path.addRect(-barrier_size/2, -barrier_size/2, barrier_size, barrier_size);
    return path;
}

void BarrierElem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::red);
    painter->drawEllipse(QRectF(-barrier_size/2, -barrier_size/2, barrier_size, barrier_size));
}
