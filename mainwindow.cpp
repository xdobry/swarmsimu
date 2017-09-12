#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRectF>
#include <QTimer>
#include "schwarmelem.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene.setSceneRect(-500, -300, 1000, 600);

    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->graphicsView->show();

    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), &scene, SLOT(advance()));

    ui->param1->setValue(scene.schwarmAlgorithm.getParam1());
    ui->param2->setValue(scene.schwarmAlgorithm.getParam1());
    ui->spinSpeed->setValue(scene.schwarmAlgorithm.getSpeed());
    timer->start(1000 / framesProSecond);
    isRuning = true;
}

MainWindow::~MainWindow()
{
    delete ui;
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
    } else {
        ui->btnStartStop->setText("Stop");
        isRuning = true;
        timer->start(1000/framesProSecond);
    }
}

void MainWindow::on_btnClear_clicked()
{
    scene.clear();
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
