#ifndef VECMATH_H
#define VECMATH_H

#include <qglobal.h>

double getAngleDegress(double dx,double dy);
qreal getAngleAbs(qreal angle1,qreal angle2);
qreal normalizeAngle(qreal angle);
/**
 * Berechne winkel relativ zu anderen winkel
 * z.B 20 grad is zu 10 relativ 10, und zu 30 ist es -10.
 * @brief relativeAngel
 * @param angle
 * @param relativeAngle
 * @return
 */
qreal relativeAngle(qreal angle,qreal relativeAngle);

qreal vectorSpeed(qreal dx,qreal dy);
void rotateVector(qreal &dx,qreal &dy,qreal angleDegrees);
void scaleVector(qreal &dx,qreal &dy,qreal scale);
void addVectorLen(qreal &dx, qreal &dy,qreal acc);
void setVectorLen(qreal &dx, qreal &dy,qreal neededLen);
qreal pointDistance (qreal x1,qreal y1,qreal x2, qreal y2);



#endif // VECMATH_H
