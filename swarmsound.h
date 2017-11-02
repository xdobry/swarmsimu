#ifndef SWARMSOUND_H
#define SWARMSOUND_H

#include <QObject>
#include <QAudioOutput>
#include <QAudioFormat>
#include <QIODevice>
#include <QAudioDeviceInfo>
#include <QDebug>
#include <QList>
#include <QThread>
#include <QPointer>

#include <math.h>
#include <iostream>
#include <random>

#include <schwarmelem.h>

void initSinTable();

class ElemAudioSource {
public:
    ElemAudioSource(SchwarmElem *schwarmElemPar): schwarmElem(schwarmElemPar) {}
    void readData(qreal *ldata,qreal *rdata, qint64 len,qreal level);
    bool isValid();

private:
    QPointer<SchwarmElem> schwarmElem;
    // position in perio 0..2PI
    qreal anglePos = -1;
    // Own Freqency Derivation from standard
    qreal ownDeviation = 0;
    qreal lvelocity = 0;
    qreal rvelocity = 0;
    // period increment for one frame step (scale freqence of stream to 0..2PI Freqeuncy)
    qreal delta = -1;

    qreal anglePos2 = -1;
    qreal lvelocity2 = 0;
    qreal rvelocity2 = 0;
    qreal delta2 = -1;

    qreal oldvx = -1000;
    qreal oldvy = -1000;
};

class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format, qint64 durationUs, QObject *parent);
    ~Generator();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    void addAudioElemData(SchwarmElem *schwarmElem);
    void resetElements();

private:
    void generateData();


private:
    qint64 m_pos;
    qint64 samplesCount;
    QByteArray m_buffer;
    QByteArray mixleft_buffer;
    QByteArray mixright_buffer;
    QList<ElemAudioSource *> audioSources;
    qint64 maxElapsedTime = 0;
};

struct AudioElemData {
    short int velocity;
    short int acceleration;
    short int panorama;
};

class SwarmSound : public QThread
{
    Q_OBJECT

public:
    SwarmSound(QObject *parent);
    void addAudioElemData(SchwarmElem *schwarmElem);
    void startSound();
    void stopSound();
    void resetSound();
    void suspend();
    void resume();

protected:
    void run();

private:
    QAudioOutput* _audio;
    Generator *m_generator;
};

#endif // SWARMSOUND_H
