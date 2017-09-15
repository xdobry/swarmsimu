#ifndef SCHWARMALGORITHM_H
#define SCHWARMALGORITHM_H

#include <QList>

class SchwarmElem;
class BarrierElem;
class PoiElem;

qreal pointDistance(qreal x1,qreal y1,qreal x2, qreal y2);

struct NextSchwarmElem {
    SchwarmElem *schwarmElem;
    double distance;
};

struct CollisionData;

struct CollisionSchwarmElem;

class SchwarmAlgorithm {
public:
    virtual void advance(SchwarmElem &schwarmElem) = 0;
};

class BaseSchwarmAlgorithm : public SchwarmAlgorithm {
public:
    void advance(SchwarmElem &schwarmElem) override;
};

class ParamSchwarmAlgorithm : public SchwarmAlgorithm {
protected:
    // optimal distance
    double param1 = 15;
    double param2 = 3;
    bool showSeeField = false;
    bool deleteSeeField = false;
    double angleCorrectur = 2;
    bool endlessWord = true;
    double normalSpeed = 3;
    double acceleration = 0.2;
public:
    ParamSchwarmAlgorithm(double p1,double p2);
    void setParam1(double param);
    void setParam2(double param);
    double getParam1();
    double getParam2();
    void setShowSeeField(bool param);
    bool getShowSeeField();
    void setEndlessWord(bool param);
    bool getEndlessWord();
    void setSpeed(double param);
    double getSpeed();
};


class CurvingSchwarmAlgorithm : public ParamSchwarmAlgorithm {
public:
    CurvingSchwarmAlgorithm(double p1,double p2);
    void advance(SchwarmElem &schwarmElem) override;
};

class InteractSchwarmAlgorithm : public ParamSchwarmAlgorithm {
public:
    InteractSchwarmAlgorithm(double p1,double p2);
    void advance(SchwarmElem &schwarmElem) override;

private:
    qreal avoidBorder(SchwarmElem &schwarmElem, qreal crotation);
    void adaptInSwarm(SchwarmElem &schwarmElem, double ownAngle, QList<NextSchwarmElem> &farSchwarm, const QList<NextSchwarmElem> &nearSchwarm);
    void adaptForCollisions(SchwarmElem &schwarmElem,double ownAngle,const QList<CollisionSchwarmElem> &collisionSchwarm,int countOwnSchwarm);
    void adaptForBarriers(SchwarmElem &schwarmElem,const QList<BarrierElem*> &barrierElems);
    void adaptForPois(SchwarmElem &schwarmElem);
    void moveElem(SchwarmElem &schwarmElemW);
    qreal getOptimalDistance();
};


#endif // SCHWARMALGORITHM_H
