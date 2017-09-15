#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include "schwarmelem.h"
#include "schwarmalgorithm.h"
#include "swarmscene.h"
#include <list>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openFile();

    void saveFile();

    void saveFileAs();

    void on_btnStart_clicked();

    void on_param1_valueChanged(double arg1);

    void on_param2_valueChanged(double arg1);

    void on_btnAddMore_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_btnStartStop_clicked();

    void on_btnClear_clicked();

    void on_chbSeeField_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

    void on_spinSpeed_valueChanged(double arg1);

    void on_rdbElem_toggled(bool checked);

    void on_rdbPOI_toggled(bool checked);

    void on_rdbBarrier_toggled(bool checked);

private:
    void params2gui();

    Ui::MainWindow *ui;
    SwarmScene scene;
    int framesProSecond = 33;
    QTimer *timer;
    bool isRuning;
    QString savedFile;
};

#endif // MAINWINDOW_H
