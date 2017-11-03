#include "swarmsound.h"
#include <qendian.h>
// only speed test
#include "swarmscene.h"

#include <vecmath.h>

#include <QElapsedTimer>

const int DurationMSec = 200;

std::default_random_engine generator;
std::normal_distribution<double> distribution(0.0,2.0);



areal *sinTable = NULL;
areal *sawTable = NULL;
const int sampleRate = 44100;
const int sinTableSize = sampleRate/10;

void initSinTable() {
    if (!sinTable) {
        sinTable = new areal[sinTableSize];
        for (int i = 0;i<sinTableSize;i++) {
            sinTable[i] = sin((areal)i*(2.0*M_PI)/(areal)sinTableSize);
        }
    }
    if (!sawTable) {
        sawTable = new areal[sinTableSize];
        for (int i = 0;i<sinTableSize;i++) {
            sawTable[i] = -2.0*(areal)i/(areal)sinTableSize+0.5;
        }
    }
}

areal sinFromTable(areal x) {
    if (sinTable) {
        int tpos = (int)(x*(areal)sinTableSize/(2.0*M_PI))%sinTableSize;
        return sinTable[tpos];
        //return sinTable[((qint64)(x*(double)sinTableSize/(2.0*M_PI)))%sinTableSize];
    } else {
        return sin(x);
    }
}

areal sawFromTable(areal x) {
    if (sawTable) {
        int tpos = (int)(x*(areal)sinTableSize/(2.0*M_PI))%sinTableSize;
        return sawTable[tpos];
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
    // speedTests();
}

void SwarmSound::speedTests() {
    QElapsedTimer timer;
    areal ownFrequency = (50.0+ 390.0 * 0.2);
    areal cdelta = 2.0 * M_PI * ownFrequency / 44100.0;
    areal pos = 0.0;
    int samplesCount = 441000;
    QByteArray buffer;
    buffer.resize(samplesCount*sizeof(areal));
    buffer.fill(0);
    areal *mix = reinterpret_cast<areal *>(buffer.data());
    timer.start();
    for (int i=0;i<samplesCount;i++) {
        areal value = sinFromTable(pos);
        *mix += value;
        mix++;
        pos+=cdelta;
    }
    qDebug() << "1 sec from table: " << timer.elapsed() << "milliseconds";
    mix = reinterpret_cast<areal *>(buffer.data());
    timer.restart();
    pos = 0.0;
    for (int i=0;i<samplesCount;i++) {
        areal value = sin(pos);
        *mix += value;
        mix++;
        pos+=cdelta;

    }
    qDebug() << "1 sec from function: " << timer.elapsed() << "milliseconds";
    QAudioFormat format;
    format.setSampleRate(sampleRate);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    Generator generator(format,DurationMSec,this);
    InteractSchwarmAlgorithm schwarmAlgorithm = InteractSchwarmAlgorithm(15,0.05);
    QList<SchwarmElem *> elems;
    int count = 50;
    for  (int i = 0;i<20;i++) {
        SchwarmElem *s1 = new SchwarmElem(0,0,2,2,schwarmAlgorithm);
        elems.append(s1);
        generator.addAudioElemData(s1);
    }
    timer.restart();
    generator.generateData();
    generator.generateData();
    generator.generateData();
    generator.generateData();
    generator.generateData();
    qDebug() << "generator call " << timer.elapsed() << "milliseconds by count: "<<count;
    foreach (SchwarmElem *elem, elems) {
        delete elem;
    }
}

void SwarmSound::stopSound()
{
    _audio->stop();
    m_generator->stop();
    m_generator->resetElements();
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
        mixright_buffer.resize(samplesCount*sizeof(areal));
        mixleft_buffer.resize(samplesCount*sizeof(areal));
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

void Generator::generateData() {
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int length = samplesCount * 2 * 2;
    generateDataPtr(ptr,length);
}


void Generator::generateDataPtr(unsigned char *ptr, int length)
{
    const int channelBytes = 2;
    samplesCount = length/4;

    int needBufferLen = samplesCount*sizeof(areal);
    if (mixleft_buffer.size()<needBufferLen) {
        mixleft_buffer.resize(needBufferLen);
    }
    if (mixright_buffer.size()<needBufferLen) {
        mixright_buffer.resize(needBufferLen);
    }

    mixleft_buffer.fill(0);
    mixright_buffer.fill(0);

    areal *leftmix = reinterpret_cast<areal *>(mixleft_buffer.data());
    areal *rightmix = reinterpret_cast<areal *>(mixright_buffer.data());

    int audioSourcesLenght = audioSources.size();
    if (audioSourcesLenght>0) {
        int maxaudio = 200;
        areal level = 1.0/qreal(qMin(maxaudio,audioSourcesLenght)); // *log(audioSources.length()+1);
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
            areal qvalue = (*leftmix++);
            if (qvalue>1.0) {
                qvalue=1.0;
            } else if (qvalue<-1.0) {
                qvalue = -1.0;
            }
            /*
            slowAudio += slowAudioDiff;
            if (slowAudio>1.0 || slowAudio<0.0) {
                slowAudioDiff = -slowAudioDiff;
            }
            qvalue *= slowAudio;
            */

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
            // qvalue *= slowAudio;

            value = static_cast<qint16>(qvalue * 32767);
            qToLittleEndian<qint16>(value, ptr);
            ptr += channelBytes;
            length -= channelBytes;
        }
    } else {
        memset(ptr,0,length);
    }
}

void Generator::addAudioElemData(SchwarmElem *schwarmElem)
{
    audioSources.push_back(new ElemAudioSource(schwarmElem));
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
    QElapsedTimer timer;
    timer.start();
    generateDataPtr((unsigned char *)data,len);
    qint64 timeElapsed = timer.elapsed();
    timeReport++;
    if (timeReport>50) {
        maxElapsedTime = 0;
    }
    if (timeElapsed>maxElapsedTime) {
        qDebug() << "audio data processing took" << timeElapsed << "milliseconds" << " len: "<< len << " elem count: "<<audioSources.size();
        maxElapsedTime = timeElapsed;
        timeReport = 0;
    }
    //qDebug()<<"readdata len:"<<len;
    /*
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
                // QThread::msleep(50);
                qint64 timeElapsed = timer.elapsed();
                timeReport++;
                if (timeReport>50) {
                    maxElapsedTime = 0;
                }
                if (timeElapsed>maxElapsedTime) {
                    qDebug() << "The slow operation took" << timeElapsed << "milliseconds" << " len: "<< len << " elem count: "<<audioSources.length();
                    maxElapsedTime = timeElapsed;
                    timeReport = 0;
                }
                // break;
            }
            total += chunk;
        }
    }
    */
    return len;
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

void ElemAudioSource::readData(areal *ldata, areal *rdata, qint64 len,areal level)
{
    areal rlevel = 0;
    areal llevel = 0;
    areal rlevel2 = 0;
    areal llevel2 = 0;
    // TODO scene width

    areal sceneWidth = 500;
    areal audioLevel = 0.2 * level;

    int stressPoints = 0;
    if (schwarmElem->mode==crowded) {
        stressPoints++;
    }
    if (schwarmElem->mode==collision) {
        stressPoints+=2;
    }
    areal accStress = 0.0;
    if (anglePos>0.0) {
        accStress = vectorSpeed(oldvx-schwarmElem->vx,oldvy-schwarmElem->vy);
    }
    oldvx = schwarmElem->vx;
    oldvy = schwarmElem->vy;

    areal stressAudioLevel = level * 0.1 * stressPoints + accStress*0.02;

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
        pos = ((areal)rand()/RAND_MAX)*2.0 * M_PI;
        ownDeviation = distribution(generator);
    }
    areal ownFrequency = (50.0+ 390.0 * rSpeed + ownDeviation);
    areal cdelta = 2.0 * M_PI * ownFrequency / 44100.0;

    areal sinvalue = 0;

    // The volumume differency will be distributed for 1/2 of length
    int halbLen = len/2;
    areal crlevel = rvelocity;
    areal cllevel = lvelocity;
    areal rdelta = (rlevel-rvelocity)/qreal(halbLen);
    areal ldelta = (llevel-lvelocity)/qreal(halbLen);
    areal sdelta;
    areal ddelta;
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
    areal sawvalue = 0;
    areal cdelta2 = 2.0 * M_PI * ownFrequency * 1.5 / 44100.0;

    areal pos2 = anglePos2;
    if (pos2<0) {
        // Set init position to random
        pos2 = ((qreal)rand()/RAND_MAX)*2.0 * M_PI;
    }

    areal crlevel2 = rvelocity2;
    areal cllevel2 = lvelocity2;
    areal rdelta2 = (rlevel2-rvelocity2)/qreal(halbLen);
    areal ldelta2 = (llevel2-lvelocity2)/qreal(halbLen);
    areal sdelta2;
    areal ddelta2;
    // The frequency differency will be distributed to full length
    if (delta2>0) {
        sdelta2 = delta2;
        ddelta2 = (cdelta2-delta2)/areal(len);
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
    anglePos = fmod(pos,areal(2.0 * M_PI));
    anglePos2 = fmod(pos2,areal(2.0 * M_PI));
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
