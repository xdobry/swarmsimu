#include "schwarmalgorithm.h"
#include "schwarmelem.h"
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QRectF>
#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
//static double TwoPi = 2.0 * Pi;
static const double minColTime = 60;
static const double optimalDistance = 15;

/**
 * -180..180
 * @brief getAngleDegress
 * @param dx
 * @param dy
 * @return
 */
double getAngleDegress(double dx,double dy) {
  return atan2(dx, -dy)*180.0/Pi;
}

qreal vectorSpeed(qreal dx,qreal dy) {
    return sqrt(dx*dx+dy*dy);
}

void rotateVector(qreal &dx,qreal &dy,qreal angleDegrees) {
    qreal angleRadians = angleDegrees*Pi/180;
    qreal dxtemp = dx;
    dx = dx * cos(angleRadians) - dy * sin(angleRadians);
    dy = dxtemp * sin(angleRadians) + dy * cos(angleRadians);
}

void scaleVector(qreal &dx,qreal &dy,qreal scale) {
    dx*=scale;
    dy*=scale;
}

void addVectorLen(qreal &dx, qreal &dy,qreal acc) {
    qreal len = vectorSpeed(dx,dy);
    if (len>0) {
        dx += dx/len*acc;
        dy += dy/len*acc;
    } else {
        // Create random vx, vy
        qreal vmax = 10;
        qreal vx = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
        qreal vy = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
        len = vectorSpeed(dx,dy);
        dx = dx/len*acc;
        dy = dy/len*acc;
    }
}

void setVectorLen(qreal &dx, qreal &dy,qreal neededLen) {
    qreal len = vectorSpeed(dx,dy);
    if (len>0) {
        dx = dx/len*neededLen;
        dy = dy/len*neededLen;
    }
}

qreal getAngleAbs(qreal angle1,qreal angle2) {
    qreal diff = angle1-angle2;
    if (diff<0) {
        diff = -diff;
    }
    //diff = remainder(diff,360);
    if (diff>180) {
        diff = 360-diff;
    }
    return diff;
}

qreal normalizeAngle(qreal angle) {
    angle = fmod(angle,360);
    if (angle<0) {
        angle+=360;
    }
    return angle;
}

qreal pointDistance (qreal x1,qreal y1,qreal x2, qreal y2) {
    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

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
            delete item;
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
            int countPossibleSchwarm = 0;
            qreal sx = schwarmElem.vx;
            qreal sy = schwarmElem.vy;
            QList<SchwarmElem*> possibleSchwarm;
            QList<NextSchwarmElem> nearSchwarm;
            QList<NextSchwarmElem> farSchwarm;
            QList<CollisionSchwarmElem> collisions;
            //qDebug("own pos x=%lf, y=%lf",schwarmElem.x(),schwarmElem.y());
            bool hasAlreadyNear = false;
            qreal ownAngle = getAngleDegress(sx,sy);
            qreal avgNextSpeedSum = 0;
            int countForNextSpeedSum = 0;
            foreach (QGraphicsItem *item, nearItems) {
               if (item == &schwarmElem) {
                   continue;
               }
               SchwarmElem *nextSchwarmElem = dynamic_cast<SchwarmElem*>(item);
               if (nextSchwarmElem) {
                   if (schwarmElem.mode==alone) {
                      schwarmElem.mode = see;
                   }
                   schwarmCount++;
                   qreal nAngle = getAngleDegress(nextSchwarmElem->vx,nextSchwarmElem->vy);
                   qreal absAngle = getAngleAbs(ownAngle,nAngle);
                   // qreal distance = pointDistance(nextSchwarmElem->x(),nextSchwarmElem->y(),schwarmElem.x(),schwarmElem.y());
                   qreal nextDistance = pointDistance(nextSchwarmElem->x()+nextSchwarmElem->vx,
                                                      nextSchwarmElem->y()+nextSchwarmElem->vy,
                                                      schwarmElem.x()+sx,
                                                      schwarmElem.y()+sy);
                   //qDebug("next pos x=%lf, y=%lf",nextSchwarmElem->x(),nextSchwarmElem->y());
                   if (absAngle<10) {
                       if (nextDistance<optimalDistance) {
                           // korriegieren abstand zu klein
                           nearSchwarm.append({nextSchwarmElem,nextDistance});
                           hasAlreadyNear = true;
                           schwarmElem.mode = crowded;
                       } else if (nextDistance>25 && !hasAlreadyNear) {
                           farSchwarm.append({nextSchwarmElem,nextDistance});
                       } else {
                           if (schwarmElem.mode==see) {
                               schwarmElem.mode = swarm;
                           }
                           countForNextSpeedSum++;
                           avgNextSpeedSum += vectorSpeed(nextSchwarmElem->vx,nextSchwarmElem->vy);
                       }
                       countOwnSchwarm++;
                   } else if (absAngle<45) {
                       possibleSchwarm.append(nextSchwarmElem);
                       countPossibleSchwarm++;
                   } else {
                       // Ausweichen wenn nötig (Kollisionskurs)
                       CollisionData collisionData = schwarmElem.computeCollisionDistance(*nextSchwarmElem);
                       if (collisionData.distance>=0 && collisionData.distance<schwarmElem.spread()*3 && collisionData.time<=minColTime) {
                           collisions.append({nextSchwarmElem,collisionData});
                           schwarmElem.mode = collision;
                       }
                   }
               }
            }
            if (collisions.size()>0) {
                adaptForCollisions(schwarmElem,ownAngle,collisions,countOwnSchwarm);
            } else if (nearSchwarm.size()>0) {
                adaptTooNear(schwarmElem,ownAngle,nearSchwarm);
            } else if (farSchwarm.size()>0) {
                schwarmElem.mode = farswarm;
                adaptTooFar(schwarmElem,ownAngle,farSchwarm);
            } else if (possibleSchwarm.size()>0 && countOwnSchwarm==0) {
                addToSchwarm(schwarmElem,possibleSchwarm);
            } else {
                if (countForNextSpeedSum>0) {
                    qreal nextSpeedAvg = avgNextSpeedSum/countForNextSpeedSum;
                    qreal ownSpeed = vectorSpeed(sx,sy);
                    if (nextSpeedAvg>ownSpeed+acceleration/2) {
                        addVectorLen(schwarmElem.vx,schwarmElem.vy,acceleration/2);
                    } else if (nextSpeedAvg<ownSpeed-acceleration/2) {
                        addVectorLen(schwarmElem.vx,schwarmElem.vy,-acceleration/2);
                    }
                }
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
    if (schwarmElem.vrotation!=0) {
       rotateVector(schwarmElem.vx,schwarmElem.vy,schwarmElem.vrotation);
    }
    schwarmElem.setRotation(getAngleDegress(schwarmElem.vx,schwarmElem.vy));
    moveElem(schwarmElem);
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
            schwarmElem.vrotation = crotation * preferedRotationSign * 180 / Pi;
        } else {
           schwarmElem.vrotation =  (schwarmElem.vrotation > 0 ? crotation : -crotation) * 180 / Pi;
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


void InteractSchwarmAlgorithm::adaptTooNear(SchwarmElem &schwarmElem,double ownAngle,const QList<NextSchwarmElem> &nearSchwarm) {
    qreal distance = -1;
    SchwarmElem* nearestElem;
    qreal dx = 0;
    qreal dy = 0;
    // only correct to nearest element
    foreach (NextSchwarmElem elem, nearSchwarm) {
        if (elem.distance<optimalDistance) {
            distance = elem.distance;
            qreal gravity = 0.08*(optimalDistance-elem.distance)*(optimalDistance-elem.distance);
            qreal edx = schwarmElem.x()-elem.schwarmElem->x();
            qreal edy = schwarmElem.y()-elem.schwarmElem->y();
            setVectorLen(edx,edy,gravity);
            dx += edx;
            dy += edy;
        }
    }
    schwarmElem.vx += dx;
    schwarmElem.vy += dy;
}

void InteractSchwarmAlgorithm::adaptForCollisions(SchwarmElem &schwarmElem,double ownAngle,const QList<CollisionSchwarmElem> &collisionSchwarm,int countOwnSchwarm) {
    qreal time = -1;
    CollisionSchwarmElem *nearestElem;
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

    if (time<10) {
        correktur = correktur*2;
    }
    rotateVector(schwarmElem.vx,schwarmElem.vy,correktur);
    if (time<5) {
        qreal ownSpeed = vectorSpeed(schwarmElem.vx,schwarmElem.vy);
        if (ownSpeed-acceleration>0) {
            addVectorLen(schwarmElem.vx,schwarmElem.vy,-acceleration);
        }
    }
}

void InteractSchwarmAlgorithm::adaptTooFar(SchwarmElem &schwarmElem,double ownAngle,const QList<NextSchwarmElem> &farSchwarm) {
    qreal distance = 1000;
    SchwarmElem *nextSchwarmElem;
    // only correct to nearest element
    foreach (NextSchwarmElem elem, farSchwarm) {
         if (elem.distance<distance) {
             nextSchwarmElem=elem.schwarmElem;
         }
    }
    int adaptSign = 1;
    if (ownAngle>=-45 && ownAngle<45) {
        if (schwarmElem.x()>nextSchwarmElem->x()) {
            adaptSign = -1;
        }
    } else if (ownAngle>45 && ownAngle<=135) {
        if (schwarmElem.y()>nextSchwarmElem->y()) {
            adaptSign = -1;
        }
    } else if (ownAngle>135 && ownAngle<-135) {
        if (schwarmElem.x()<nextSchwarmElem->x()) {
            adaptSign = -1;
        }
    } else if (ownAngle<-45 && ownAngle>=-135) {
        if (schwarmElem.y()<nextSchwarmElem->y()) {
            adaptSign = -1;
        }
    }
    qreal sx = schwarmElem.vx;
    qreal sy = schwarmElem.vy;
    rotateVector(sx,sy,adaptSign*angleCorrectur);
    qreal nextSpeed = vectorSpeed(nextSchwarmElem->vx,nextSchwarmElem->vy);
    qreal ownSpeed = vectorSpeed(sx,sy);
    if (nextSpeed>ownSpeed-acceleration) {
         addVectorLen(sx,sy,acceleration);
    }
    schwarmElem.vx = sx;
    schwarmElem.vy = sy;
}

void InteractSchwarmAlgorithm::addToSchwarm(SchwarmElem &schwarmElem, const QList<SchwarmElem *> &possibleSchwarm)
{
    schwarmElem.mode = annexation;
    // Anpassen an durchschnitt des Schwarms. Eingliedern
    qreal sx = schwarmElem.vx;
    qreal sy = schwarmElem.vy;
    foreach (SchwarmElem *schwarmItem, possibleSchwarm) {
        sx = (sx+schwarmItem->vx)/2;
        sy = (sy+schwarmItem->vy)/2;
    }
    schwarmElem.vx = sx;
    schwarmElem.vy = sy;
}


void InteractSchwarmAlgorithm::test()
{
    qDebug("getAngleDegress(1,0.002)=%f test=%f",getAngleDegress(1,0.002),90.02);
    qDebug("getAngleDegress(-1.02,1.2)=%lf",getAngleDegress(-1.02,1.2));
    qDebug("getAngleAbs(-10,10)=%lf",getAngleAbs(-10,10));
    qDebug("getAngleAbs(10,20)=%lf",getAngleAbs(10,20));
    qDebug("getAngleAbs(-90,91)=%lf",getAngleAbs(-90,91));
    qDebug("vectorSpeed(1,-1)=%lf",vectorSpeed(1,1));
    qreal x=1, y=1;
    scaleVector(x,y,vectorSpeed(2,2)/vectorSpeed(1,1));
    qDebug("after scale x=%lf y=%lf",x,y);
    rotateVector(x,y,90);
    qDebug("after rotate 90 x=%lf y=%lf",x,y);

}
