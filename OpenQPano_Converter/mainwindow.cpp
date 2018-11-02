#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <ctime>
#include "ili9225.h"
#include "convert.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentWidget(ui->tabLCD);
    tIdCap = 0;
    csx = PIN_LCD_CSA;

    ui->pbxSrc->setScaledContents(true);   // 影像自動縮放至Label尺寸
}

MainWindow::~MainWindow()
{
    LCD_Close();

    if ( tIdCap != 0 )
        killTimer(tIdCap);

    delete ui;
}


void MainWindow::timerEvent( QTimerEvent *event )
{
//    cap >> imgSrc;

//    if(imgSrc.empty()){
//        QMessageBox::critical(NULL, "Error", "No Camera!");
//        killTimer(tIdCap);
//        return;
//    }

//    ShowFrame(imgSrc, ui->pbxS);
}

void MainWindow::on_btnLoad_clicked()
{
    ui->btnLoad->setEnabled(false);
    fileName = QFileDialog::getOpenFileName(this,tr("Open File")); // 開啟檔案對話盒並取得檔名
    imgSrc = imread(fileName.toStdString(),IMREAD_COLOR);          // 讀入影像

     if(!imgSrc.empty()) {                                         // 若影像不空
         if( (imgSrc.rows != LCD_HEIGHT) ||
             (imgSrc.cols != LCD_WIDTH) ){
             cv::resize(imgSrc, imgSrc, Size(LCD_WIDTH, LCD_HEIGHT), 0, 0, INTER_LINEAR);
             QMessageBox::information(this,"Information","Image will resize to 176x220.");
         }

         ShowFrame(imgSrc, ui->pbxSrc);                            // 將圖框影像秀在標籤上
         ui->btnShow->setEnabled(true);
         ui->btnClear->setEnabled(true);
     }
     else{
         QMessageBox::critical(this,"Error","Image open fail !");
         return;
     }
     ui->btnLoad->setEnabled(true);
}

void MainWindow::on_btnShow_clicked()
{
     ui->btnShow->setEnabled(false);
     Mat imgLCD = Mat::zeros(Size(imgSrc.cols,imgSrc.rows),CV_8UC2);

     BGR8882RGB565(imgSrc, imgLCD);
     ILI9225_Show_Data(csx, imgLCD.ptr<uint8_t>(0),
                       0, 0, imgLCD.cols, imgLCD.rows);
     ui->btnShow->setEnabled(true);
}

void MainWindow::on_btnClear_clicked()
{
    ui->btnClear->setEnabled(false);
    ILI9225_Clr_Screen(csx);
    ui->btnClear->setEnabled(true);
}

void MainWindow::on_btnColorBar_clicked()
{
    ui->btnColorBar->setEnabled(false);
    ILI9225_LCD_Test(csx);
    ui->btnColorBar->setEnabled(true);
}

void MainWindow::on_btnPlay_clicked()
{
    ui->btnPlay->setEnabled(false);
    fileName = QFileDialog::getOpenFileName(this,tr("Open File")); // 開啟檔案對話盒並取得檔名

    VideoCapture capSrc = VideoCapture(fileName.toStdString());
    int cap_w = capSrc.get(CV_CAP_PROP_FRAME_WIDTH);
    int cap_h = capSrc.get(CV_CAP_PROP_FRAME_HEIGHT);
    int cap_f = capSrc.get(CV_CAP_PROP_FRAME_COUNT);
    Mat imgLCD = Mat::zeros(Size(cap_w,cap_h),CV_8UC2);
    double t0,t1,t2,t3,fps;

    t0 = (double)getTickCount();

    if(capSrc.isOpened()) {
        while(1){            
            capSrc >> imgSrc;

            if(imgSrc.empty())
                break;

            //ShowFrame(imgSrc, ui->pbxSrc);  // 將圖框影像秀在標籤上
            t1 = (double)getTickCount();
            BGR8882RGB565(imgSrc, imgLCD);
            t2 = (double)getTickCount();

            ILI9225_Show_Data(csx, imgLCD.ptr<uint8_t>(0),
                              0, 0, imgLCD.cols, imgLCD.rows);
//            delay(10); // frame interval
            t3= (double)getTickCount();
        }

        fps = 1.0 / ((t3 - t0)/getTickFrequency()/cap_f);

        QString strInfo = "Play done, \ncovert time = " +
                          QString::number((double)(t2 - t1)/getTickFrequency()*1000) +
                          "ms, \ndisplay time = " +
                          QString::number((double)(t3 - t2)/getTickFrequency()*1000) +
                          "ms, \ntotal time = " +
                          QString::number((double)(t3 - t0)/getTickFrequency()*1000) +
                          "ms, \n" + QString::number(fps) + " FPS";
        QMessageBox::information(this,"Information", strInfo);
     }
     else{
         QMessageBox::critical(this,"Error","Video open fail !");
         return;
     }

    ui->btnPlay->setEnabled(true);
}

void MainWindow::on_btnInital_clicked()
{
    ui->btnInital->setEnabled(false);
    lcd_state = LCD_Init(csx);

    if(lcd_state!=0){
        QMessageBox::critical(this,"Error","LCD inital fail !");
        exit(0);
    }
    else{
        ILI9225_LCD_Test(csx);
        ui->btnLoad->setEnabled(true);
        ui->btnClear->setEnabled(true);
        ui->btnColorBar->setEnabled(true);
        ui->btnPlay->setEnabled(true);
        ui->btnPlayOmni->setEnabled(true);
        ui->ckbIOTest->setEnabled(true);
        ui->btnConvert->setEnabled(true);
        ui->btnShowPano->setEnabled(true);
        ui->btnPlayPano->setEnabled(true);
    }

    ui->btnInital->setEnabled(true);
}

void MainWindow::on_rdbCS1_clicked()
{
    csx = PIN_LCD_CS1;
}

void MainWindow::on_rdbCS2_clicked()
{
    csx = PIN_LCD_CS2;
}

void MainWindow::on_rdbCS3_clicked()
{
    csx = PIN_LCD_CS3;
}

void MainWindow::on_rdbCS4_clicked()
{
    csx = PIN_LCD_CS4;
}

void MainWindow::on_rdbCS5_clicked()
{
    csx = PIN_LCD_CS5;
}

void MainWindow::on_rdbCS6_clicked()
{
    csx = PIN_LCD_CS6;
}

void MainWindow::on_rdbCSA_clicked()
{
    csx = PIN_LCD_CSA;
}

void MainWindow::on_ckbIOTest_clicked()
{
    ui->ckbCS1->setEnabled(ui->ckbIOTest->isChecked());
    ui->ckbRS->setEnabled(ui->ckbIOTest->isChecked());
    ui->ckbWR->setEnabled(ui->ckbIOTest->isChecked());
    ui->ckbRST->setEnabled(ui->ckbIOTest->isChecked());
    ui->ckbBL->setEnabled(ui->ckbIOTest->isChecked());
    ui->ckbDB0->setEnabled(ui->ckbIOTest->isChecked());
}

void MainWindow::on_ckbCS1_clicked()
{
    if(ui->ckbCS1->isChecked())
        ILI9225_CS(PIN_LCD_CS1, HIGH);
    else
        ILI9225_CS(PIN_LCD_CS1, LOW);
}

void MainWindow::on_ckbRS_clicked()
{
    if(ui->ckbRS->isChecked())
        ILI9225_IO_Test(PIN_LCD_RS, HIGH);
    else
        ILI9225_IO_Test(PIN_LCD_RS, LOW);
}

void MainWindow::on_ckbWR_clicked()
{
    if(ui->ckbWR->isChecked())
        ILI9225_IO_Test(PIN_LCD_WR, HIGH);
    else
        ILI9225_IO_Test(PIN_LCD_WR, LOW);
}

void MainWindow::on_ckbDB0_clicked()
{
    if(ui->ckbDB0->isChecked())
        ILI9225_WR_Data8(0xAA);
    else
        ILI9225_WR_Data8(0x55);
}

void MainWindow::on_ckbRST_clicked()
{
    if(ui->ckbRST->isChecked())
        ILI9225_IO_Test(PIN_LCD_RST, HIGH);
    else
        ILI9225_IO_Test(PIN_LCD_RST, LOW);
}

void MainWindow::on_ckbBL_clicked()
{
    if(ui->ckbBL->isChecked())
        LCD_Backlight(HIGH);
    else
        LCD_Backlight(LOW);
}

void MainWindow::on_btnPlayOmni_clicked()
{
    ui->btnPlayOmni->setEnabled(false);

    fileName = QFileDialog::getOpenFileName(this,tr("Open File"));
    QFileInfo info(fileName);
    QString videoPath = info.path();
    QString videoName[LCD_AMOUNT] = {"LCD_0.avi", "LCD_1.avi", "LCD_2.avi",
                            "LCD_3.avi", "LCD_4.avi", "LCD_5.avi"};
    VideoCapture capSrc[LCD_AMOUNT];
    Mat imgLCD;
    uint8_t cs_pin[LCD_AMOUNT] = {PIN_LCD_CS1, PIN_LCD_CS2, PIN_LCD_CS3,
                                  PIN_LCD_CS4, PIN_LCD_CS5, PIN_LCD_CS6};

    int cap_w, cap_h, cap_f;
    double t0,t1,fps;

    for(int n=0; n<LCD_AMOUNT; n++){
        fileName = videoPath + "/" + videoName[n];
        capSrc[n] = VideoCapture(fileName.toStdString());

        if(!capSrc[n].isOpened()){
            QString strErr = "Video capture" + QString::number(n) + " open fail !";
            QMessageBox::critical(this,"Error", strErr);
            return;
        }
    }

    cap_w = capSrc[0].get(CV_CAP_PROP_FRAME_WIDTH);
    cap_h = capSrc[0].get(CV_CAP_PROP_FRAME_HEIGHT);
    cap_f = capSrc[0].get(CV_CAP_PROP_FRAME_COUNT);
    imgLCD = Mat::zeros(Size(cap_w,cap_h),CV_8UC2);
    t0 = (double)getTickCount();

    for(int f=0; f<cap_f; f++){
        for(int n=0; n<LCD_AMOUNT; n++){
            capSrc[n] >> imgSrc;

            if(imgSrc.empty()){
                QString strErr = "Video capture" + QString::number(n) +
                                 " frme[" + QString::number(f) +
                                 "] import fail !";
                QMessageBox::critical(this,"Error", strErr);
                ui->btnPlayOmni->setEnabled(true);
                return;
            }

            BGR8882RGB565(imgSrc, imgLCD);
            ILI9225_Show_Data(cs_pin[n], imgLCD.ptr<uint8_t>(0),
                              0, 0, imgLCD.cols, imgLCD.rows);
        }
    }

    t1= (double)getTickCount();
    fps = 1.0 /((t1 - t0)/getTickFrequency()/cap_f);

    QString strInfo = "Play omni done, \ntotal time = " +
                      QString::number((double)(t1 - t0)/getTickFrequency()) +
                      "sec, \n" + QString::number(fps) + " FPS";
    QMessageBox::information(this,"Information", strInfo);

    ui->btnPlayOmni->setEnabled(true);
}

void MainWindow::on_btnIOTest_clicked()
{

}

void MainWindow::on_btnConvert_clicked()
{
    double t0,t1,t2,t3;

    ui->btnConvert->setEnabled(false);

    fileName = QFileDialog::getOpenFileName(this,tr("Open File")); // 開啟檔案對話盒並取得檔名
    QFileInfo info(fileName);
    QString ext = info.completeSuffix();

    double view_theta = 2.0 * CV_PI / 3.0; // 60*2 degree
    Mat imgPano;

    if(ext == "jpg" || ext == "JPG" || ext == "bmp" || ext == "BMP" ||
       ext == "png" || ext == "PNG" ){
        t0= (double)getTickCount();

        imgSrc = imread(fileName.toStdString(),IMREAD_COLOR);          // 讀入影像

        t1= (double)getTickCount();

        if(!imgSrc.empty()) {                                         // 若影像不空
           if(imgSrc.cols == imgSrc.rows*2){

             PanoConvert(imgSrc,view_theta,LCD_WIDTH,LCD_HEIGHT,LCD_AMOUNT,imgPano);
             t2= (double)getTickCount();

             imwrite("pano.png", imgPano);
             t3= (double)getTickCount();

             QString strInfo =
             "Image read time = " + QString::number((double)(t1 - t0)/getTickFrequency()) + "sec, \n" +
             "Pano convert time = " + QString::number((double)(t2 - t1)/getTickFrequency()) + "sec, \n" +
             "Image write time = " + QString::number((double)(t3 - t2)/getTickFrequency()) + "sec, \n" +
             "Total time = " + QString::number((double)(t3 - t0)/getTickFrequency()) + "sec.";
             QMessageBox::information(this,"Information", strInfo);
           }
           else{
             QMessageBox::critical(this,"Error","Image size mismatch !");
           }
         }
         else{
             QMessageBox::critical(this,"Error","Image open fail !");
         }
    }
    else if(ext == "mp4" || ext == "MP4" || ext == "avi" || ext == "AVI" ||
            ext == "wmv" || ext == "WMV" || ext == "mpg" || ext == "MPG" ){
          VideoCapture capSrc = VideoCapture(fileName.toStdString());

          if(!capSrc.isOpened()){
              QString strErr = "Video file open fail !";
              QMessageBox::critical(this,"Error", strErr);
              ui->btnConvert->setEnabled(true);
              return;
          }

          long cap_f = capSrc.get(CV_CAP_PROP_FRAME_COUNT);

          VideoWriter writer;
          Mat frame;

//          writer.open("pano.avi", CV_FOURCC('M', 'J', 'P', 'G'),
//                      30, cv::Size(LCD_WIDTH*LCD_AMOUNT,LCD_HEIGHT)); // *.mp4(DIVX)

          writer.open("pano.mp4", CV_FOURCC('D', 'I', 'V', 'X'),
                      30, cv::Size(LCD_WIDTH*LCD_AMOUNT,LCD_HEIGHT)); // *.avi(MJPG)

          ui->pgbConvert->setValue(0);
          ui->pgbConvert->setMaximum(cap_f);
          t0= (double)getTickCount();

          for(long f=0; f<cap_f; f++){
              capSrc >> frame;

              if(frame.empty()){
                  QString strErr = " frme[" + QString::number(f) +
                                   "] import fail !";
                  QMessageBox::critical(this,"Error", strErr);
                  ui->btnConvert->setEnabled(true);
                  return;
              }

              PanoConvert(frame,view_theta,LCD_WIDTH,LCD_HEIGHT,LCD_AMOUNT,imgPano);
              writer << imgPano;  //writer.write(imgPano);
              ui->pgbConvert->setValue(f+1);
              ui->pgbConvert->update();
          }

          t1= (double)getTickCount();

          QString strInfo =
          "Total frames = " + QString::number(cap_f) + "\n" +
          "Pano convert average time = " + QString::number((double)(t1 - t0)/getTickFrequency()/(double)cap_f) + "sec, \n" +
          "Total time = " + QString::number((double)(t1 - t0)/getTickFrequency()) + "sec.";
          QMessageBox::information(this,"Information", strInfo);
    }
    else{
        QMessageBox::critical(this,"Error","Unknow file !");
    }

    ui->btnConvert->setEnabled(true);
}

void MainWindow::on_btnShowPano_clicked()
{
    double t1,t2,t3;

    ui->btnShowPano->setEnabled(false);

    fileName = QFileDialog::getOpenFileName(this,tr("Open File")); // 開啟檔案對話盒並取得檔名
    Mat imgPano = imread(fileName.toStdString(),IMREAD_COLOR);     // 讀入影像

     if(!imgPano.empty()) {                                         // 若影像不空
       if((imgPano.cols == 1056) && (imgPano.rows == 220)){
         t1= (double)getTickCount();
         Mat imgLCD = Mat::zeros(Size(imgPano.cols,imgPano.rows),CV_8UC2);

         BGR8882RGB565(imgPano, imgLCD);

         t2= (double)getTickCount();

         ILI9225_Show_Pano(imgLCD, ui->ckbDir->isChecked()); // true for outside to inside
         t3= (double)getTickCount();

         QString strInfo =
         "LCD convert time = " + QString::number((double)(t2 - t1)/getTickFrequency()) + "sec, \n" +
         "Pano show time = " + QString::number((double)(t3 - t2)/getTickFrequency()) + "sec, \n" +
         "Total time = " + QString::number((double)(t3 - t1)/getTickFrequency()) + "sec.";
         QMessageBox::information(this,"Information", strInfo);
       }
       else{
         QMessageBox::critical(this,"Error","Image size mismatch !");
       }
     }
     else{
         QMessageBox::critical(this,"Error","Image open fail !");
     }

    ui->btnShowPano->setEnabled(true);
}


void MainWindow::on_btnPlayPano_clicked()
{
    ui->btnPlayPano->setEnabled(false);

    VideoCapture capSrc;
    uint8_t cs_pin[LCD_AMOUNT] = {PIN_LCD_CS1, PIN_LCD_CS2, PIN_LCD_CS3,
                                  PIN_LCD_CS4, PIN_LCD_CS5, PIN_LCD_CS6};

    int cap_w, cap_h, cap_f;
    double t0,t1,fps;

    fileName = QFileDialog::getOpenFileName(this,tr("Open File"));
    capSrc = VideoCapture(fileName.toStdString());

    if(!capSrc.isOpened()){
        QString strErr = "Video file open fail !";
        QMessageBox::critical(this,"Error", strErr);
        ui->btnPlayPano->setEnabled(true);
        return;
    }

    cap_w = capSrc.get(CV_CAP_PROP_FRAME_WIDTH);
    cap_h = capSrc.get(CV_CAP_PROP_FRAME_HEIGHT);
    cap_f = capSrc.get(CV_CAP_PROP_FRAME_COUNT);

    if((cap_w != LCD_WIDTH*LCD_AMOUNT) || (cap_h != LCD_HEIGHT)){
        QString strErr = "Video size fail !";
        QMessageBox::critical(this,"Error", strErr);
        ui->btnPlayPano->setEnabled(true);
        return;
    }

    Mat imgOri;
    Mat imgLCD = Mat::zeros(Size(LCD_WIDTH*LCD_AMOUNT,LCD_HEIGHT),CV_8UC2);
    Mat imgFrame = Mat::zeros(Size(LCD_WIDTH,LCD_HEIGHT),CV_8UC2);

    uint8_t *ptrS;
    uint8_t *ptrT;

    ui->pgbConvert->setMaximum(cap_f);
    ui->pgbConvert->setValue(0);
    ui->pgbConvert->update();

    t0 = (double)getTickCount();

    for(int f=0; f<cap_f; f++){
        capSrc >> imgOri;

        if(imgOri.empty()){
            QString strErr = " frme[" + QString::number(f) +
                             "] import fail !";
            QMessageBox::critical(this,"Error", strErr);
            ui->btnPlayPano->setEnabled(true);
            return;
        }

        BGR8882RGB565(imgOri, imgLCD);

        for(int n=0; n<LCD_AMOUNT; n++){
            for(int y=0; y<imgLCD.rows; y++){
                ptrS = imgLCD.ptr<uint8_t>(y) + (n * LCD_WIDTH * 2);
                ptrT = imgFrame.ptr<uint8_t>(y);

                for(int x=0; x<LCD_WIDTH*2; x++){
                    ptrT[x] = ptrS[x];
                }
            }

            ILI9225_Show_Data(cs_pin[n], imgFrame.ptr<uint8_t>(0),
                              0, 0, imgFrame.cols, imgFrame.rows);

        }

        ui->pgbConvert->setValue(f+1);
        ui->pgbConvert->update();

        if(bcm2835_gpio_lev(PIN_PB1) == LOW){
            bcm2835_gpio_write(PIN_LED1, HIGH);
            usleep(100000); // 100ms
            cout << "OK" << endl;
            bcm2835_gpio_write(PIN_LED1, LOW);
            LCD_Close();
            exit(0);
        }
    }

    t1= (double)getTickCount();
    fps = 1.0 /((t1 - t0)/cap_f/getTickFrequency());

    QString strInfo = "Play pano done, \ntotal time = " +
                      QString::number((double)(t1 - t0)/getTickFrequency()) +
                      "sec, \n" + QString::number(fps) + " FPS";
    QMessageBox::information(this,"Information", strInfo);

    ui->btnPlayPano->setEnabled(true);
}

