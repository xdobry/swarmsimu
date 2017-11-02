#include "swarmscene.h"
#include "schwarmelem.h"

#include <QGraphicsSceneMouseEvent>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <vecmath.h>


void SwarmScene::addSwarmItem()
{
    qreal vmax = 5;
    qreal xmax = 100;
    qreal ymax = 20;
    qreal x = ((qreal)rand()/RAND_MAX)*xmax-xmax/2;
    qreal y = ((qreal)rand()/RAND_MAX)*ymax-ymax/2;
    qreal vx = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
    qreal vy = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
    SchwarmElem *schwarm = new SchwarmElem(x,y,vx,vy,schwarmAlgorithm);
    addSwarmElem(schwarm);

    /*
    SchwarmElem *schwarm = new SchwarmElem(5,0,0.4,0.0,schwarmAlgorithm);
    addItem(schwarm);
    schwarm = new SchwarmElem(-25,0,0.6,0.2,schwarmAlgorithm);
    addItem(schwarm);
    */
    //schwarm = new SchwarmElem(-50,0,0.7,0.0,schwarmAlgorithm);
     //addItem(schwarm);
}

void SwarmScene::setAddElemType(SwarmScene::ScharmElemAddType schwarmElemAddTypeArg)
{
    schwarmElemAddType = schwarmElemAddTypeArg;
}

SwarmScene::ScharmElemAddType SwarmScene::getAddElemType()
{
    return schwarmElemAddType;
}

void SwarmScene::openFile(const QString &fileName)
{
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString message =  QString("Couldn't open ") + fileName + QString(" to load settings for download");
        QMessageBox::critical(NULL,"Load XML File Problem",message,QMessageBox::Ok);
            return;
    }
    QXmlStreamReader *xmlReader = new QXmlStreamReader(&xmlFile);
    clearSwarm();

    //Parse the XML until we reach end of it
    double x,y,vx,vy;

    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument) {
                continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement) {
            x=0;
            y=0;
            vx=0;
            vy=0;
            if(xmlReader->name() == "item") {
                foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                     if (attr.name().toString() == QLatin1String("x")) {
                         QString value = attr.value().toString();
                         x = value.toDouble();
                     } else if (attr.name().toString() == QLatin1String("y")) {
                         QString value = attr.value().toString();
                         y = value.toDouble();
                     } else if (attr.name().toString() == QLatin1String("vx")) {
                         QString value = attr.value().toString();
                         vx = value.toDouble();
                     } else if (attr.name().toString() == QLatin1String("vy")) {
                         QString value = attr.value().toString();
                         vy = value.toDouble();
                     }
                }
                addSwarmElem(new SchwarmElem(x,y,vx,vy,schwarmAlgorithm));
             } else if(xmlReader->name() == "barrier") {
                foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                     if (attr.name().toString() == QLatin1String("x")) {
                         QString value = attr.value().toString();
                         x = value.toDouble();
                     } else if (attr.name().toString() == QLatin1String("y")) {
                         QString value = attr.value().toString();
                         y = value.toDouble();
                     }
                }
                addItem(new BarrierElem(x,y));
             } if(xmlReader->name() == "poi") {
                foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                     if (attr.name().toString() == QLatin1String("x")) {
                         QString value = attr.value().toString();
                         x = value.toDouble();
                     } else if (attr.name().toString() == QLatin1String("y")) {
                         QString value = attr.value().toString();
                         y = value.toDouble();
                     }
                }
                addItem(new PoiElem(x,y));
             } if(xmlReader->name() == "algorithm") {
                foreach(const QXmlStreamAttribute &attr, xmlReader->attributes()) {
                     if (attr.name().toString() == QLatin1String("speed")) {
                         QString value = attr.value().toString();
                         schwarmAlgorithm.setSpeed(value.toDouble());
                     } else if (attr.name().toString() == QLatin1String("param1")) {
                         QString value = attr.value().toString();
                         schwarmAlgorithm.setParam1(value.toDouble());
                     } else if (attr.name().toString() == QLatin1String("param2")) {
                         QString value = attr.value().toString();
                         schwarmAlgorithm.setParam2(value.toDouble());
                     } else if (attr.name().toString() == QLatin1String("endlessWorld")) {
                         QString value = attr.value().toString();
                         schwarmAlgorithm.setEndlessWord(value=="true");
                     }
                }
             }
        }
    }

    if(xmlReader->hasError()) {
            QMessageBox::critical(NULL,
            "xmlFile.xml Parse Error",xmlReader->errorString(),
            QMessageBox::Ok);
            return;
    }

    //close reader and flush file
    xmlReader->clear();
    delete xmlReader;
    xmlFile.close();
}

bool SwarmScene::saveFile(const QString &fileName)
{
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString message =  QString("Couldn't open ") + fileName + QString(" to write swarm data");
        QMessageBox::critical(NULL,"Load XML File Problem",message,QMessageBox::Ok);
            return false;
    }
    QXmlStreamWriter *xmlWriter = new QXmlStreamWriter(&xmlFile);
    xmlWriter->setAutoFormatting(true);
    xmlWriter->writeStartDocument();

    xmlWriter->writeStartElement("swarm");

    xmlWriter->writeStartElement("algorithm");
    xmlWriter->writeAttribute("endlessWorld",schwarmAlgorithm.getEndlessWord() ? "true" : "false");
    xmlWriter->writeAttribute("speed",QString::number(schwarmAlgorithm.getSpeed()));
    xmlWriter->writeAttribute("param1",QString::number(schwarmAlgorithm.getParam1()));
    xmlWriter->writeAttribute("param2",QString::number(schwarmAlgorithm.getParam2()));
    xmlWriter->writeEndElement();

    xmlWriter->writeStartElement("scene");

    foreach (QGraphicsItem *item, items()) {
        SchwarmElem *schwarmElem = dynamic_cast<SchwarmElem*>(item);
        if (schwarmElem) {
            xmlWriter->writeStartElement("item");
            xmlWriter->writeAttribute("x",QString::number(schwarmElem->x()));
            xmlWriter->writeAttribute("y",QString::number(schwarmElem->y()));
            xmlWriter->writeAttribute("vx",QString::number(schwarmElem->vx));
            xmlWriter->writeAttribute("vy",QString::number(schwarmElem->vy));
            xmlWriter->writeEndElement();
        } else {
            BarrierElem *barrierElem = dynamic_cast<BarrierElem*>(item);
            if (barrierElem) {
                xmlWriter->writeStartElement("barrier");
                xmlWriter->writeAttribute("x",QString::number(barrierElem->x()));
                xmlWriter->writeAttribute("y",QString::number(barrierElem->y()));
                xmlWriter->writeEndElement();
            } else {
                PoiElem *poiElem = dynamic_cast<PoiElem*>(item);
                if (poiElem) {
                    xmlWriter->writeStartElement("poi");
                    xmlWriter->writeAttribute("x",QString::number(poiElem->x()));
                    xmlWriter->writeAttribute("y",QString::number(poiElem->y()));
                    xmlWriter->writeEndElement();
                }
            }
        }
    }

    xmlWriter->writeEndElement();
    xmlWriter->writeEndElement();
    xmlWriter->writeEndDocument();
    xmlFile.close();
    delete xmlWriter;
    return true;
}

void SwarmScene::initSound(QObject *parent)
{
    if (!swarmSound) {
        initSinTable();
        swarmSound = new SwarmSound(parent);
    }
    foreach (QGraphicsItem *item, items()) {
        SchwarmElem *schwarmElem = dynamic_cast<SchwarmElem*>(item);
        if (schwarmElem) {
            swarmSound->addAudioElemData(schwarmElem);
        }
    }
}

void SwarmScene::clearSwarm()
{
    clear();
    if (swarmSound) {
        swarmSound->resetSound();
    }
}

void SwarmScene::suspend()
{
    if (swarmSound) {
        swarmSound->suspend();
    }
}

void SwarmScene::resume()
{
    if (swarmSound) {
        swarmSound->resume();
    }

}

void SwarmScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        mousePressPosition = mouseEvent->scenePos();
        hasPressed = false;
        if (schwarmElemAddType==poi) {
            addItem(new PoiElem(mousePressPosition.x(),mousePressPosition.y()));
        } else if (schwarmElemAddType==barrier) {
            addItem(new BarrierElem(mousePressPosition.x(),mousePressPosition.y()));
        } else {
            hasPressed = true;
        }
    } else if (mouseEvent->button() == Qt::RightButton) {
        // Delete item under cursor
        QGraphicsItem *selectedItem = itemAt(mouseEvent->scenePos(),QTransform());
        if (selectedItem) {
            removeItem(selectedItem);
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void SwarmScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        if (hasPressed) {
            QPointF endMouse = mouseEvent->scenePos();
            qreal vx = (endMouse.x()-mousePressPosition.x())/5;
            qreal vy = (endMouse.y()-mousePressPosition.y())/5;
            if (vx==0 && vy==0) {
                // we need movement
                qreal vmax = 1;
                vx = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
                vy = ((qreal)rand()/RAND_MAX)*vmax-vmax/2;
            }
            SchwarmElem *schwarm = new SchwarmElem(mousePressPosition.x(),mousePressPosition.y(),vx,vy,schwarmAlgorithm);
            addSwarmElem(schwarm);
        }
        hasPressed = false;
    }
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void SwarmScene::addSwarmElem(SchwarmElem *schwarmElem)
{
    addItem(schwarmElem);
    if (swarmSound) {
        swarmSound->addAudioElemData(schwarmElem);
    }
}
