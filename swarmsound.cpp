#include "swarmsound.h"
#include <qendian.h>

#include <vecmath.h>

#include <QElapsedTimer>

const int DurationMSec = 100;

std::default_random_engine generator;
std::normal_distribution<double> distribution(0.0,2.0);

qreal *sinTable = NULL;
qreal *sawTable = NULL;
const int sampleRate = 44100;
const int sinTableSize = sampleRate/10;

void initSinTable() {
    if (!sinTable) {
        sinTable = new qreal[sinTableSize];
        for (int i = 0;i<sinTableSize;i++) {
            sinTable[i] = sin((qreal)i*(2.0*M_PI)/(qreal)sinTableSize);
        }
    }
    if (!sawTable) {
        sawTable = new qreal[sinTableSize];
        for (int i = 0;i<sinTableSize;i++) {
            sawTable[i] = -2.0*(qreal)i/(qreal)sinTableSize+0.5;
        }
    }
}

qreal sinFromTable(qreal x) {
    if (sinTable) {
        return sinTable[((qint64)(x*(qreal)sinTableSize/(2.0*M_PI)))%sinTableSize];
    } else {
        return sin(x);
    }
}

qreal sawFromTable(qreal x) {
    if (sawTable) {
        return sawTable[((qint64)(x*(qreal)sinTableSize/(2.0*M_PI)))%sinTableSize];
    } else {
        return 0;
    }
}


SwarmSound::SwarmSound(QObject *parent) : QThread(parent)
{
    QAudioFormat format;
    // set up the format you want, eg.
    format.setSampleRate(sampleRate);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        qWarning()<<"default format not supported try to use nearest";
        format = info.nearestFormat(format);
    }
    _audio = new QAudioOutput(format);
    // _audio->setBufferSize(44100);//in bytes
    m_generator = new Generator(format, DurationMSec, this);

}

void SwarmSound::addAudioElemData(SchwarmElem *schwarmElem)
{
    m_generator->addAudioElemData(schwarmElem);
}


void SwarmSound::startSound()
{
    m_generator->start();
    _audio->start(m_generator);
}

void SwarmSound::stopSound()
{
    m_generator->stop();
    _audio->stop();
}

void SwarmSound::resetSound()
{
    _audio->suspend();
    m_generator->resetElements();
    _audio->resume();
}

void SwarmSound::suspend()
{
    _audio->suspend();
}

void SwarmSound::resume()
{
    _audio->resume();
}

void SwarmSound::run()
{
    startSound();
    exec();
    qDebug()<<"Swarm sound run finished";
}

Generator::Generator(const QAudioFormat &formatp,
                     qint64 durationMsec,
                     QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0)
{
    if (formatp.isValid()) {
        const int channelBytes = formatp.sampleSize() / 8;
        const int sampleBytes = formatp.channelCount() * channelBytes;
        samplesCount = formatp.sampleRate() * durationMsec / 1000;
        qint64 length = samplesCount * formatp.channelCount() * (formatp.sampleSize() / 8);
        Q_ASSERT(length % sampleBytes == 0);
        Q_UNUSED(sampleBytes) // suppress warning in release builds
        qDebug()<<"read buffer "<<length << " samples count "<<samplesCount;
        m_buffer.resize(length);
        mixright_buffer.resize(samplesCount*sizeof(qreal));
        mixleft_buffer.resize(samplesCount*sizeof(qreal));
        //generateData();
    }
}

Generator::~Generator()
{
    resetElements();
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::generateData()
{
    const int channelBytes = 2;
    qint64 length = samplesCount * 2 * 2;
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());

    mixleft_buffer.fill(0);
    mixright_buffer.fill(0);

    qreal *leftmix = reinterpret_cast<qreal *>(mixleft_buffer.data());
    qreal *rightmix = reinterpret_cast<qreal *>(mixright_buffer.data());

    if (audioSources.length()>0) {
        int maxaudio = 5;
        qreal level = 1.0/qreal(qMin(maxaudio,audioSources.length())); // *log(audioSources.length()+1);
        int elemCount = 0;
        foreach (ElemAudioSource *elemAudioSource , audioSources) {
           if (elemAudioSource->isValid()) {
               elemAudioSource->readData(leftmix,rightmix,samplesCount,level);
               elemCount++;
               if (elemCount>maxaudio) {
                   break;
               }
           }
        }
        while (length>0) {
            // const qreal x = sin(2 * M_PI * sampleRate * qreal(sampleIndex % format.sampleRate()) / format.sampleRate());
            qreal qvalue = (*leftmix++);
            if (qvalue>1.0) {
                qvalue=1.0;
            } else if (qvalue<-1.0) {
                qvalue = -1.0;
            }
            qint16 value = static_cast<qint16>(qvalue * 32767);
            qToLittleEndian<qint16>(value, ptr);
            ptr += channelBytes;
            length -= channelBytes;
            if (length==0) {
                qDebug()<<"Fuck";
            }
            qvalue = (*rightmix++);
            if (qvalue>1.0) {
                qvalue = 1.0;
            } else if (qvalue<-1.0) {
                qvalue = -1.0;
            }
            value = static_cast<qint16>(qvalue * 32767);
            qToLittleEndian<qint16>(value, ptr);
            ptr += channelBytes;
            length -= channelBytes;
        }
    } else {
        m_buffer.fill(0);
    }
}

void Generator::addAudioElemData(SchwarmElem *schwarmElem)
{
    audioSources.append(new ElemAudioSource(schwarmElem));
}

void Generator::resetElements()
{
    foreach (ElemAudioSource *elemAudioSource , audioSources) {
       delete elemAudioSource;
    }
    audioSources.clear();
}

qint64 Generator::readData(char *data, qint64 len)
{
    //qDebug()<<"readdata len:"<<len;
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            if (m_pos==0) {
                //qDebug()<<"buf start"<<samplesCount;
                QElapsedTimer timer;
                timer.start();
                generateData();
                qint64 timeElapsed = timer.elapsed();
                if (timeElapsed>maxElapsedTime) {
                    qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds" << " len: "<< len;
                    maxElapsedTime = timeElapsed;
                }
                // break;
            }
            total += chunk;
        }
    }

    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

void ElemAudioSource::readData(qreal *ldata, qreal *rdata, qint64 len,qreal level)
{
    qreal rlevel = 0;
    qreal llevel = 0;
    qreal rlevel2 = 0;
    qreal llevel2 = 0;
    // TODO scene width

    qreal sceneWidth = 500;
    qreal audioLevel = 0.2 * level;

    int stressPoints = 0;
    if (schwarmElem->mode==crowded) {
        stressPoints++;
    }
    if (schwarmElem->mode==collision) {
        stressPoints+=2;
    }
    qreal accStress = 0.0;
    if (anglePos>0.0) {
        accStress = vectorSpeed(oldvx-schwarmElem->vx,oldvy-schwarmElem->vy);
    }
    oldvx = schwarmElem->vx;
    oldvy = schwarmElem->vy;

    qreal stressAudioLevel = level * 0.1 * stressPoints + accStress*0.02;

    rlevel = (schwarmElem->x()+sceneWidth)/1000.0*audioLevel;
    llevel = (sceneWidth-schwarmElem->x())/1000.0*audioLevel;
    rlevel2 = (schwarmElem->x()+sceneWidth)/1000.0*stressAudioLevel;
    llevel2 = (sceneWidth-schwarmElem->x())/1000.0*stressAudioLevel;

    //qDebug()<<"rlevel: "<<rlevel<<" llevel: "<<llevel;

    qreal speed = vectorSpeed(schwarmElem->vx,schwarmElem->vy);
    qreal rSpeed = speed / 10;

    qreal pos = anglePos;
    if (pos<0) {
        // Set init position to random
        pos = ((qreal)rand()/RAND_MAX)*2.0 * M_PI;
        ownDeviation = distribution(generator);
    }
    qreal ownFrequency = (50.0+ 390.0 * rSpeed + ownDeviation);
    qreal cdelta = 2.0 * M_PI * ownFrequency / 44100.0;

    qreal sinvalue = 0;

    // The volumume differency will be distributed for 1/2 of length
    int halbLen = len/2;
    qreal crlevel = rvelocity;
    qreal cllevel = lvelocity;
    qreal rdelta = (rlevel-rvelocity)/qreal(halbLen);
    qreal ldelta = (llevel-lvelocity)/qreal(halbLen);
    qreal sdelta;
    qreal ddelta;
    // The frequency differency will be distributed to full length
    if (delta>0) {
        sdelta = delta;
        ddelta = (cdelta-delta)/qreal(len);
    } else {
        sdelta = cdelta;
        ddelta = 0;
    }

    // Wiederholung für Stress Ton (säge)
    // 1.5 entspricht der quite C auf G
    qreal sawvalue = 0;
    qreal cdelta2 = 2.0 * M_PI * ownFrequency * 1.5 / 44100.0;

    qreal pos2 = anglePos2;
    if (pos2<0) {
        // Set init position to random
        pos2 = ((qreal)rand()/RAND_MAX)*2.0 * M_PI;
    }

    qreal crlevel2 = rvelocity2;
    qreal cllevel2 = lvelocity2;
    qreal rdelta2 = (rlevel2-rvelocity2)/qreal(halbLen);
    qreal ldelta2 = (llevel2-lvelocity2)/qreal(halbLen);
    qreal sdelta2;
    qreal ddelta2;
    // The frequency differency will be distributed to full length
    if (delta2>0) {
        sdelta2 = delta2;
        ddelta2 = (cdelta2-delta2)/qreal(len);
    } else {
        sdelta2 = cdelta;
        ddelta2 = 0;
    }

    for (qint64 i = 0; i<len; i++) {
        sinvalue = sinFromTable(pos);
        //qreal rsinvalue = sin(pos);
        //if (abs(sinvalue-rsinvalue)>0.01) {
        //    qDebug()<<"shit";
        //}
        *rdata+=crlevel*sinvalue;
        *ldata+=cllevel*sinvalue;

        if (crlevel2>0.0 || cllevel2>0.0) {
            sawvalue = sawFromTable(pos2);
            *rdata+=crlevel2*sawvalue;
            *ldata+=cllevel2*sawvalue;
        }

        ldata++;
        rdata++;

        pos+=sdelta;
        sdelta+=ddelta;

        pos2+=sdelta2;
        sdelta2+=ddelta2;

        if (halbLen>0) {
            halbLen--;
            crlevel += rdelta;
            cllevel += ldelta;

            crlevel2 += rdelta2;
            cllevel2 += ldelta2;
        }
    }
    anglePos = fmod(pos,qreal(2.0 * M_PI));
    anglePos2 = fmod(pos2,qreal(2.0 * M_PI));
    // anglePos = pos;
    rvelocity = crlevel;
    lvelocity = cllevel;
    delta = cdelta;

    rvelocity2 = crlevel2;
    lvelocity2 = cllevel2;
    delta2 = cdelta2;

}

bool ElemAudioSource::isValid()
{
  return schwarmElem;
}
