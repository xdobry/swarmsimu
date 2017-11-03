#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRectF>
#include <QTimer>
#include <QFileDialog>
#include "schwarmelem.h"
//#include <QGLWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene.setSceneRect(-500, -300, 1000, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    //ui->graphicsView->setViewport(new QGLWidget());

    ui->graphicsView->show();

    params2gui();
    ui->statusBar->showMessage("left click and drag (for speed vector) to add swarm element. Right click to remove");

    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    timer->start(1000 / framesProSecond);

    isRuning = true;
}

MainWindow::~MainWindow()
{
    timer->stop();
    scene.stopSound();
    delete ui;
    delete timer;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Swarm File", "/home/jana", "Swarm File (*.xml)");
    if (!fileName.isNull()) {
        timer->stop();
        scene.openFile(fileName);
        timer->start();
        params2gui();
    }
}

void MainWindow::saveFile()
{
    if (!savedFile.isNull()) {
        scene.saveFile(savedFile);
    }
}

void MainWindow::saveFileAs()
{
    QString nameToSave = "swarm.xml";
    if (!savedFile.isNull()) {
        nameToSave = savedFile;
    }
    QString fileName = QFileDialog::getSaveFileName(this,"Open Swarm File", nameToSave , "Swarm File (*.xml)");
    if (!fileName.isNull()) {
        if (scene.saveFile(fileName)) {
            savedFile = fileName;
        }
    }
}

void MainWindow::on_btnStart_clicked()
{
    scene.addSwarmItem();
}

void MainWindow::on_param1_valueChanged(double arg1)
{
    scene.schwarmAlgorithm.setParam1(arg1);
}

void MainWindow::on_param2_valueChanged(double arg1)
{
    scene.schwarmAlgorithm.setParam2(arg1);
}

void MainWindow::on_btnAddMore_clicked()
{
    for (int i=0;i<20;i++) {
        scene.addSwarmItem();
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    framesProSecond = value;
    if (isRuning) {
        timer->start(1000/framesProSecond);
    }
}



void MainWindow::on_btnStartStop_clicked()
{
    if (isRuning) {
        isRuning = false;
        timer->stop();
        ui->btnStartStop->setText("Start");
        scene.suspend();
    } else {
        ui->btnStartStop->setText("Stop");
        isRuning = true;
        timer->start(1000/framesProSecond);
        scene.resume();
    }
}

void MainWindow::on_btnClear_clicked()
{
    scene.clearSwarm();
}

void MainWindow::on_chbSeeField_toggled(bool checked)
{
    scene.schwarmAlgorithm.setShowSeeField(checked);
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    scene.schwarmAlgorithm.setEndlessWord(checked);
}

void MainWindow::on_spinSpeed_valueChanged(double speed)
{
    scene.schwarmAlgorithm.setSpeed(speed);
}

void MainWindow::on_rdbElem_toggled(bool checked)
{
    if (checked) {
        scene.setAddElemType(SwarmScene::swarm);
    }
}

void MainWindow::on_rdbPOI_toggled(bool checked)
{
    if (checked) {
        scene.setAddElemType(SwarmScene::poi);
    }
}

void MainWindow::on_rdbBarrier_toggled(bool checked)
{
    if (checked) {
        scene.setAddElemType(SwarmScene::barrier);
    }
}

void MainWindow::params2gui()
{
    ui->param1->setValue(scene.schwarmAlgorithm.getParam1());
    ui->param2->setValue(scene.schwarmAlgorithm.getParam2());
    ui->spinSpeed->setValue(scene.schwarmAlgorithm.getSpeed());

}

void MainWindow::on_chbSound_toggled(bool checked)
{
    scene.initSound(this);
    if (checked) {
        // Start in separate thread (this will not work)
        // scene.swarmSound->start();
        scene.startSound();
    } else {
        scene.stopSound();
    }
}

