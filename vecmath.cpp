#include "vecmath.h"
#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;

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

/**
 * Berechne winkel relativ zu anderen winkel
 * z.B 20 grad is zu 10 relativ 10, und zu 30 ist es -10.
 * @brief relativeAngel
 * @param angle
 * @param relativeAngle
 * @return
 */
qreal relativeAngle(qreal angle,qreal relativeAngle) {
    return normalizeAngle(normalizeAngle(angle)-normalizeAngle(relativeAngle));
}

