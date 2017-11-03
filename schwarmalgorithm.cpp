#include "schwarmalgorithm.h"
#include "schwarmelem.h"
#include "vecmath.h"
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QRectF>
#include <math.h>
#include <algorithm>
#include <QDebug>
#include <list>

static const double minColTime = 60;

void ParamSchwarmAlgorithm::setParam1(double param)
{
    param1 = param;
}

void ParamSchwarmAlgorithm::setParam2(double param)
{
    param2 = param;
}

double ParamSchwarmAlgorithm::getParam1()
{
    return param1;
}

double ParamSchwarmAlgorithm::getParam2()
{
    return param2;
}

void ParamSchwarmAlgorithm::setShowSeeField(bool param)
{
    if (!param && showSeeField) {
        deleteSeeField = true;
    } else {
        deleteSeeField = false;
    }
    showSeeField = param;
}

bool ParamSchwarmAlgorithm::getShowSeeField()
{
    return showSeeField;
}

void ParamSchwarmAlgorithm::setEndlessWord(bool param)
{
    endlessWord = param;
}

bool ParamSchwarmAlgorithm::getEndlessWord()
{
    return endlessWord;
}

void ParamSchwarmAlgorithm::setSpeed(double param)
{
    normalSpeed = param;
}

double ParamSchwarmAlgorithm::getSpeed()
{
    return normalSpeed;
}

ParamSchwarmAlgorithm::ParamSchwarmAlgorithm(double p1, double p2) : param1(p1),param2(p2)
{

}

void BaseSchwarmAlgorithm::advance(SchwarmElem &schwarmElem)
{
    qreal w = schwarmElem.scene()->width();
    qreal h = schwarmElem.scene()->height();
    qreal relx = schwarmElem.x()/(w/2);
    qreal rely = schwarmElem.y()/(h/2);
    bool wallaproach = false;
    if (schwarmElem.vx>0 && relx>0.8) {
        wallaproach = true;
        if (schwarmElem.vrotation==0) {
            if (schwarmElem.vy>0) {
                schwarmElem.vrotation = 0.1;
            } else {
                schwarmElem.vrotation = -0.1;
            }
        }
    } else if (schwarmElem.vx<0 && relx<-0.8) {
        wallaproach = true;
        if (schwarmElem.vrotation==0) {
            if (schwarmElem.vy>0) {
                schwarmElem.vrotation = -0.1;
            } else {
                schwarmElem.vrotation = 0.1;
            }
        }
    } else if (schwarmElem.vy>0 && rely>0.8) {
        wallaproach = true;
        if (schwarmElem.vrotation==0) {
            if (schwarmElem.vx>0) {
                schwarmElem.vrotation = -0.1;
            } else {
                schwarmElem.vrotation = 0.1;
            }
        }
    } else if (schwarmElem.vy<0 && rely<-0.8) {
        wallaproach = true;
        if (schwarmElem.vrotation==0) {
            if (schwarmElem.vx>0) {
                schwarmElem.vrotation = 0.1;
            } else {
                schwarmElem.vrotation = -0.1;
            }
        }
    }

    if (wallaproach) {
       rotateVector(schwarmElem.vx,schwarmElem.vy,schwarmElem.vrotation);
       if (vectorSpeed(schwarmElem.vx,schwarmElem.vy)>2) {
          scaleVector(schwarmElem.vx,schwarmElem.vy,0.98);
       }
    } else {
       schwarmElem.vrotation = 0;
       if (vectorSpeed(schwarmElem.vx,schwarmElem.vy)<20) {
          scaleVector(schwarmElem.vx,schwarmElem.vy,1.02);
       }
    }

    schwarmElem.moveBy(schwarmElem.vx,schwarmElem.vy);
}


CurvingSchwarmAlgorithm::CurvingSchwarmAlgorithm(double p1, double p2) : ParamSchwarmAlgorithm(p1,p2)
{

}

void CurvingSchwarmAlgorithm::advance(SchwarmElem &schwarmElem) {
    qreal w = schwarmElem.scene()->width();
    qreal h = schwarmElem.scene()->height();
    qreal relx = schwarmElem.x()/(w/2);
    qreal rely = schwarmElem.y()/(h/2);
    // rotation needed for border nearship
    qreal crotation = 0;
    int preferedRotationSign = 0;
    qreal curveParam = 0.3;
    if (qAbs(relx)>0.8) {
        // check if already not flying back
        if ((relx>0 && schwarmElem.vx>0) || (relx<0 && schwarmElem.vx<0)) {
            crotation = curveParam*qAbs(relx);
            preferedRotationSign = schwarmElem.vy > 0 ? 1 : -1;
            if (relx<0) {
                preferedRotationSign = -preferedRotationSign;
            }
        }
    }
    if (qAbs(rely)>0.8) {
        if ((rely>0 && schwarmElem.vy>0) || (rely<0 && schwarmElem.vy<0)) {
            crotation += curveParam*qAbs(rely);
            if (preferedRotationSign==0) {
                preferedRotationSign = schwarmElem.vx > 0 ? -1 : 1;
                if (rely<0) {
                   preferedRotationSign = -preferedRotationSign;
                }
            }
        }
    }
    if (crotation>0) {
        if (schwarmElem.vrotation==0) {
            schwarmElem.vrotation = crotation * preferedRotationSign;
        } else {
           schwarmElem.vrotation =  schwarmElem.vrotation > 0 ? crotation : -crotation;
        }
    } else {
        if (schwarmElem.vrotation<0.02) {
            schwarmElem.vrotation = 0;
        } else {
            schwarmElem.vrotation *= param2;
        }
    }
    if (schwarmElem.vrotation!=0) {
       rotateVector(schwarmElem.vx,schwarmElem.vy,schwarmElem.vrotation);
    }
    schwarmElem.moveBy(schwarmElem.vx,schwarmElem.vy);
}

InteractSchwarmAlgorithm::InteractSchwarmAlgorithm(double p1, double p2) : ParamSchwarmAlgorithm(p1,p2)
{

}


void InteractSchwarmAlgorithm::advance(SchwarmElem &schwarmElem)
{  
    // rotation needed for border nearship
    qreal crotation = 0;
    qreal optimalDistance = getOptimalDistance();
    if (!endlessWord) {
        crotation = avoidBorder(schwarmElem, crotation);
    } else {
        schwarmElem.vrotation = 0;
    }
    schwarmElem.mode = alone;
    if (deleteSeeField && !showSeeField) {
        QList<QGraphicsItem *> children = schwarmElem.childItems();
        foreach (QGraphicsItem *item, children) {
            item->setParentItem(NULL);
            schwarmElem.scene()->removeItem(item);
            // mit delete gibt es ein crash aber früher scheint das funktioniert zu haben
            // man müsste uch deleteSeeField ausschalten nach einem ganzen advance
            // das wird aber nicht gemacht
            // weil die steuerung über ganze mehtode nicht abgreifbar is (kein virtual)
            // delete item;
        }
    }
    if (crotation==0) {
        qreal nextRegion = 40;
        QPolygonF seeFeld;
        seeFeld<<QPointF(-nextRegion/2,0)<<QPointF(-nextRegion,-2*nextRegion)<<QPointF(nextRegion,-2*nextRegion)<<QPointF(nextRegion/2,0);
        if (showSeeField) {
            QList<QGraphicsItem *> children = schwarmElem.childItems();
            if (children.size()==0 && showSeeField) {
                QGraphicsPolygonItem *seeRect = schwarmElem.scene()->addPolygon(seeFeld);
                seeRect->setParentItem(&schwarmElem);
            }
        }
        QList<QGraphicsItem *> nearItems = schwarmElem.scene()->items(schwarmElem.mapToScene(seeFeld));
        int schwarmCount = 0;
        if (crotation==0 && nearItems.size()>1) {
            int countOwnSchwarm = 0;
            std::list<BarrierElem*> barrierElems;
            std::list<NextSchwarmElem> nearSchwarm;
            std::list<NextSchwarmElem> farSchwarm;
            std::list<CollisionSchwarmElem> collisions;
            qreal ownAngle = getAngleDegress(schwarmElem.vx,schwarmElem.vy);
            qreal ownSpeed = vectorSpeed(schwarmElem.vx,schwarmElem.vy);
            foreach (QGraphicsItem *item, nearItems) {
               if (item == &schwarmElem) {
                   continue;
               }
               SchwarmElem *nextSchwarmElem = dynamic_cast<SchwarmElem*>(item);
               if (nextSchwarmElem) {
                   if (schwarmElem.mode==alone) {
                      schwarmElem.mode = see;
                   }
                   qreal nAngle = getAngleDegress(nextSchwarmElem->vx,nextSchwarmElem->vy);
                   qreal absAngle = getAngleAbs(ownAngle,nAngle);
                   // qreal distance = pointDistance(nextSchwarmElem->x(),nextSchwarmElem->y(),schwarmElem.x(),schwarmElem.y());
                   qreal nextDistance = pointDistance(nextSchwarmElem->x()+nextSchwarmElem->vx,
                                                      nextSchwarmElem->y()+nextSchwarmElem->vy,
                                                      schwarmElem.x()+schwarmElem.vx,
                                                      schwarmElem.y()+schwarmElem.vy);
                   //qDebug("next pos x=%lf, y=%lf",nextSchwarmElem->x(),nextSchwarmElem->y());
                   if (nextDistance<optimalDistance) {
                       // korriegieren abstand zu klein
                       nearSchwarm.push_back({nextSchwarmElem,nextDistance});
                       if (nextDistance<optimalDistance*.07) {
                          schwarmElem.mode = crowded;
                       }
                   } else if (absAngle<45 || (ownSpeed<normalSpeed*0.1 && vectorSpeed(nextSchwarmElem->vx,nextSchwarmElem->vy)>normalSpeed/2)) {
                       schwarmCount++;
                       farSchwarm.push_back({nextSchwarmElem,nextDistance});
                       if (schwarmElem.mode==see) {
                           schwarmElem.mode = swarm;
                       }
                       countOwnSchwarm++;
                   } else {
                       // Ausweichen wenn nötig (Kollisionskurs)
                       CollisionData collisionData = schwarmElem.computeCollisionDistance(*nextSchwarmElem);
                       if (collisionData.distance>=0 && collisionData.distance<schwarmElem.spread()*3 && collisionData.time<=minColTime) {
                           collisions.push_back({nextSchwarmElem,collisionData});
                           schwarmElem.mode = collision;
                       }
                   }
               } else {
                   BarrierElem *nextBarrierElem = dynamic_cast<BarrierElem*>(item);
                   if (nextBarrierElem) {
                       barrierElems.push_back(nextBarrierElem);
                   }
               }
            }
            if (collisions.size()>0) {
                //qDebug() << "adaptForCollisions";
                adaptForCollisions(schwarmElem,ownAngle,collisions,countOwnSchwarm);
                //qDebug()<<"adaptForCollisions end";
            } else if (nearSchwarm.size()>0 || farSchwarm.size()>0) {
                //qDebug()<<"adaptInSwarm";
                adaptInSwarm(schwarmElem,ownAngle,farSchwarm,nearSchwarm);
                //qDebug()<<"adaptInSwarm End";
            }
            if (barrierElems.size()>0) {
                //qDebug()<<"adaptForBarriers";
                adaptForBarriers(schwarmElem,barrierElems);
                //qDebug()<<"adaptForBarriers end";
            }
        }
        if (schwarmCount==0) {
            // free try to accelerate
            qreal ownSpeed = vectorSpeed(schwarmElem.vx,schwarmElem.vy);
            if (ownSpeed+acceleration<normalSpeed) {
                addVectorLen(schwarmElem.vx,schwarmElem.vy,acceleration);
            } else if (ownSpeed>normalSpeed) {
                addVectorLen(schwarmElem.vx,schwarmElem.vy,-acceleration);
            }

        }
    }
    //qDebug()<<"point -1";
    if (schwarmElem.vrotation!=0) {
       rotateVector(schwarmElem.vx,schwarmElem.vy,schwarmElem.vrotation);
    }
    schwarmElem.setRotation(getAngleDegress(schwarmElem.vx,schwarmElem.vy));
    adaptForPois(schwarmElem);
    moveElem(schwarmElem);
    //qDebug()<<"advande end";
}

void InteractSchwarmAlgorithm::moveElem(SchwarmElem &schwarmElem)
{
    if (schwarmElem.vx==0 && schwarmElem.vy==0) {
        return;
    }
    if (endlessWord) {
        qreal w = schwarmElem.scene()->width();
        qreal h = schwarmElem.scene()->height();
        qreal tvx = schwarmElem.vx;
        qreal tvy = schwarmElem.vy;
        if (schwarmElem.x()+tvx>w/2) {
            tvx -= w;
        } else if (schwarmElem.x()+tvx<-w/2) {
            tvx += w;
        }
        if (schwarmElem.y()+tvy>h/2) {
            tvy -= h;
        } else if (schwarmElem.y()+tvy<-h/2) {
            tvy += h;
        }
        schwarmElem.moveBy(tvx,tvy);
    } else {
        schwarmElem.moveBy(schwarmElem.vx,schwarmElem.vy);
    }
}

qreal InteractSchwarmAlgorithm::getOptimalDistance()
{
    return param1;
}


qreal InteractSchwarmAlgorithm::avoidBorder(SchwarmElem &schwarmElem, qreal crotation)
{
    qreal w = schwarmElem.scene()->width();
    qreal h = schwarmElem.scene()->height();
    qreal relx = schwarmElem.x()/(w/2);
    qreal rely = schwarmElem.y()/(h/2);
    int preferedRotationSign = 0;
    qreal curveParam = param1;
    if (qAbs(relx)>0.8) {
        // check if already not flying back
        if ((relx>0 && schwarmElem.vx>0) || (relx<0 && schwarmElem.vx<0)) {
            crotation = curveParam*qAbs(relx);
            preferedRotationSign = schwarmElem.vy > 0 ? 1 : -1;
            if (relx<0) {
                preferedRotationSign = -preferedRotationSign;
            }
        }
    }
    if (qAbs(rely)>0.8) {
        if ((rely>0 && schwarmElem.vy>0) || (rely<0 && schwarmElem.vy<0)) {
            crotation += curveParam*qAbs(rely);
            if (preferedRotationSign==0) {
                preferedRotationSign = schwarmElem.vx > 0 ? -1 : 1;
                if (rely<0) {
                   preferedRotationSign = -preferedRotationSign;
                }
            }
        }
    }
    if (crotation>0) {
        if (schwarmElem.vrotation==0) {
            schwarmElem.vrotation = crotation * preferedRotationSign * 180 / 3.14;
        } else {
           schwarmElem.vrotation =  (schwarmElem.vrotation > 0 ? crotation : -crotation) * 180 / 3.14;
        }
    } else {
        if (schwarmElem.vrotation<10) {
            schwarmElem.vrotation = 0;
        } else {
            schwarmElem.vrotation *= (1-param2);
        }
    }
    return crotation;
}

void InteractSchwarmAlgorithm::adaptForCollisions(SchwarmElem &schwarmElem,double ownAngle,const std::list<CollisionSchwarmElem> &collisionSchwarm,int countOwnSchwarm) {
    qreal time = -1;
    CollisionSchwarmElem *nearestElem = NULL;
    //qDebug()<<"adaptForCollisions 1";
    // only correct to nearest element (the shortest time of collision)
    // wenn durch korrektur immer noch collision und nah dann abbremsen
    foreach (CollisionSchwarmElem elem, collisionSchwarm) {
         if (elem.collisionData.time<time || time<0) {
             nearestElem=&elem;
             time = elem.collisionData.time;
         }
    }
    qreal correktur = angleCorrectur;
    if (countOwnSchwarm>0) {
        correktur = correktur/2;
    }
    qreal nAngle = getAngleDegress(nearestElem->schwarmElem->vx,nearestElem->schwarmElem->vy);
    qreal collisionAngle = getAngleAbs(ownAngle,nAngle);
    if (collisionAngle<90 && normalizeAngle(ownAngle)>normalizeAngle(nAngle)) {
        correktur = -correktur;
    }

    correktur = correktur * 10 / time;
    /*
    if (time<10) {
        correktur = correktur*2;
    }
    */
    rotateVector(schwarmElem.vx,schwarmElem.vy,correktur);
    if (time<5) {
        qreal ownSpeed = vectorSpeed(schwarmElem.vx,schwarmElem.vy);
        if (ownSpeed-acceleration>0) {
            addVectorLen(schwarmElem.vx,schwarmElem.vy,-acceleration);
        }
    }
    //qDebug()<<"adaptForCollisions end";
}

void InteractSchwarmAlgorithm::adaptForBarriers(SchwarmElem &schwarmElem, const std::list<BarrierElem*> &barrierElems)
{
    qreal dx = 0;
    qreal dy = 0;
    int count = 0;
    qreal minDistance = 50;
    foreach (BarrierElem *elem, barrierElems) {
        qreal distance = pointDistance(schwarmElem.x(),schwarmElem.y(),elem->x(),elem->y());
        if (distance<minDistance) {
            //qreal gravity = 0.02*(optimalDistance-elem.distance)*(optimalDistance-elem.distance);
            qreal gravity = 0.08*(minDistance-distance);
            qreal edx = schwarmElem.x()-elem->x();
            qreal edy = schwarmElem.y()-elem->y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
            count++;
        }
    }
    if (count>0) {
        qreal ownAngle = getAngleDegress(schwarmElem.vx,schwarmElem.vy);
        qreal barrierAngle = getAngleDegress(dx,dy);
        qreal angleAbs = getAngleAbs(ownAngle,barrierAngle);
        if (angleAbs>135) {
            int rotationSign = relativeAngle(barrierAngle,ownAngle)<180 ? -1 : 1;
            rotateVector(dx,dy,90*rotationSign);
            qreal speed = schwarmElem.speed();
            schwarmElem.vx += dx;
            schwarmElem.vy += dy;
            schwarmElem.setSpeed(speed);
        }
    }
}

void InteractSchwarmAlgorithm::adaptForPois(SchwarmElem &schwarmElem)
{
    qreal dx = 0;
    qreal dy = 0;
    int count = 0;
    // Ist interessiert kommt aber nicht näher als bei minDinstance
    qreal minDistance = 30;
    foreach (PoiElem *elem, schwarmElem.poiElems) {
        qreal distance = pointDistance(schwarmElem.x(),schwarmElem.y(),elem->x(),elem->y());
        count++;
        if (distance<minDistance) {
            //qreal gravity = 0.02*(optimalDistance-elem.distance)*(optimalDistance-elem.distance);
            qreal gravity = 0.05*(minDistance-distance);
            qreal edx = schwarmElem.x()-elem->x();
            qreal edy = schwarmElem.y()-elem->y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
        } else {
            qreal gravity = 0.2*sqrt((distance-minDistance));
            qreal edx = elem->x()-schwarmElem.x();
            qreal edy = elem->y()-schwarmElem.y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
        }
    }
    if (count>0) {
        qreal speed = schwarmElem.speed();
        schwarmElem.vx += dx;
        schwarmElem.vy += dy;
        schwarmElem.setSpeed(speed);
        schwarmElem.poiElems.clear();
    }
}

bool nextScharmElemLess(const NextSchwarmElem &next1,const NextSchwarmElem &next2) {
    return next1.distance<next2.distance;
}

void InteractSchwarmAlgorithm::adaptInSwarm(SchwarmElem &schwarmElem,double ownAngle,std::list<NextSchwarmElem> &farSchwarm,const std::list<NextSchwarmElem> &nearSchwarm) {
    qreal dx = 0;
    qreal dy = 0;
    int swarmCount = 0;
    qreal sx = 0;
    qreal sy = 0;
    qreal optimalDistance = getOptimalDistance();
    foreach (NextSchwarmElem elem, nearSchwarm) {
        if (elem.distance<optimalDistance) {
            //qreal gravity = 0.02*(optimalDistance-elem.distance)*(optimalDistance-elem.distance);
            qreal gravity = 0.05*(optimalDistance-elem.distance);
            qreal edx = schwarmElem.x()-elem.schwarmElem->x();
            qreal edy = schwarmElem.y()-elem.schwarmElem->y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
            swarmCount++;
            sx+=elem.schwarmElem->vx;
            sy+=elem.schwarmElem->vy;
        }
    }
    farSchwarm.sort(nextScharmElemLess);
    //std::sort(farSchwarm.begin(),farSchwarm.end(),nextScharmElemLess);
    qreal firstDistance = -1;
    foreach (NextSchwarmElem elem, farSchwarm) {
        if (firstDistance>0 && elem.distance+optimalDistance>firstDistance) {
            // Zweite reihe, abbrechen
            break;
        }
        if (elem.distance>=optimalDistance) {
            //qreal gravity = 0.08*(elem.distance-optimalDistance)*(elem.distance-optimalDistance);
            qreal gravity = 0.05*sqrt((elem.distance-optimalDistance));
            qreal edx = elem.schwarmElem->x()-schwarmElem.x();
            qreal edy = elem.schwarmElem->y()-schwarmElem.y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
            if (firstDistance<0) {
                firstDistance = elem.distance;
            }
            swarmCount++;
            sx+=elem.schwarmElem->vx;
            sy+=elem.schwarmElem->vy;
        }
    }
    qreal aspeed = vectorSpeed(dx,dy);
    qreal maxAdaptSpeed = normalSpeed/3;
    if (aspeed>maxAdaptSpeed) {
        setVectorLen(dx,dy,maxAdaptSpeed);
    }
    if (swarmCount>0) {
        sx = sx/swarmCount;
        sy = sy/swarmCount;
    }
    // Geschwindigkeit in Relation zu swarm
    qreal rvx = sx-schwarmElem.vx;
    qreal rvy = sy-schwarmElem.vy;

    // Korrektur in der Geschwindigkeit
    dx = rvx+dx;
    dy = rvy+dy;

    schwarmElem.vx += dx;
    schwarmElem.vy += dy;
    qreal speed = vectorSpeed(schwarmElem.vx,schwarmElem.vy);
    if (speed>normalSpeed*1.3) {
        setVectorLen(schwarmElem.vx,schwarmElem.vy,normalSpeed*1.3);
    }
}
