#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>

#include <opencv2/opencv.hpp>

#define PIN_LED1     RPI_V2_GPIO_P1_08 // GPIO14, UART Tx, LED1
#define PIN_PB1      RPI_V2_GPIO_P1_10 // GPIO15, UART Rx, Push Button 1

using namespace std;
using namespace cv;

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
    void on_btnLoad_clicked();
    void on_btnShow_clicked();
    void on_btnClear_clicked();
    void on_btnColorBar_clicked();
    void on_btnPlay_clicked();
    void on_btnInital_clicked();
    void on_rdbCS1_clicked();
    void on_rdbCS2_clicked();
    void on_rdbCS3_clicked();
    void on_rdbCS4_clicked();
    void on_rdbCS5_clicked();
    void on_rdbCS6_clicked();
    void on_rdbCSA_clicked();
    void on_ckbIOTest_clicked();
    void on_ckbCS1_clicked();
    void on_ckbRS_clicked();
    void on_ckbWR_clicked();
    void on_ckbDB0_clicked();
    void on_ckbRST_clicked();
    void on_ckbBL_clicked();
    void on_btnPlayOmni_clicked();
    void on_btnIOTest_clicked();
    void on_btnConvert_clicked();
    void on_btnPlayPano_clicked();
    void on_btnShowPano_clicked();

protected:
    void timerEvent( QTimerEvent *event );
    int tIdCap;

private:
    Ui::MainWindow *ui;

    int lcd_state;
    int csx;
    QString fileName;
    Mat imgSrc;
};

#endif // MAINWINDOW_H
