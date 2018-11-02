#include "convert.h"

void ShowFrame(Mat imgS, QLabel *imgT)
{
    Mat imgC;

    cvtColor(imgS,imgC,cv::COLOR_BGR2RGB); // 轉換色彩由BGR888到RGB888

    QImage tmp(imgC.data,
               imgC.cols,
               imgC.rows,
               //imgC.step,
               QImage::Format_RGB888);
    imgT->setPixmap(QPixmap::fromImage(tmp)); // 設定QImage影像到QLabel
    imgT->update();
}

// 將來源影像cv::Mat(BGR888)轉成LCD顯示用影像(RGB565)
// imrSrc: BGR888_24bit:[B7][B6][B5][B4][B3][B2][B1][B0]
// (CV_8UC3)            [G7][G6][G5][G4][G3][G2][G1][G0]
//                      [R7][R6][R5][R4][R3][R2][R1][R0]
// imgTrg: RGB565_16bit:[R7][R6][R5][R4][R3][G7][G6][G5]
// (CV_16UC1)           [G4][G3][G2][B7][B6][B5][B4][B3]
//void BGR8882RGB565(Mat &imgSrc, Mat &imgTrg)
//{
// uint8_t *ptrS;
// uint16_t *ptrT;

// if((imgSrc.cols != imgTrg.cols) || (imgSrc.rows != imgTrg.rows)){ // 確認輸入和輸出影像尺寸要相符
//     QMessageBox::critical(this,"Error","Image size different!");
//     return;
// }

// for(int i=0; i<imgSrc.rows; i++){   // 設定迴圈數為影像高度
//     ptrS = imgSrc.ptr<uint8_t>(i);  // 取得來源影像第i列(row)起始位置指標
//     ptrT = imgTrg.ptr<uint16_t>(i); // 取得目標影像第i列(row)起始位置指標

//     for(int j=0,posS=0,posT=0; j<imgSrc.cols; j++,posS+=3, posT++){
//         ptrT[posT] = (((unsigned short int)ptrS[posS+2] & 0x00F8) << 8) | // RGB565高位元組(R5)
//                      (((unsigned short int)ptrS[posS+1] & 0x00FC) << 3) | // RGB565高/低位元組(G6)
//                      ( (unsigned short int)ptrS[posS] >> 3);              // RGB565低位元組B5)
//     }
// }
//}

// 將來源影像cv::Mat(BGR888)轉成LCD顯示用影像(RGB565)
// imrSrc: BGR888_24bit:[B7][B6][B5][B4][B3][B2][B1][B0]
// (CV_8UC3)            [G7][G6][G5][G4][G3][G2][G1][G0]
//                      [R7][R6][R5][R4][R3][R2][R1][R0]
// imgTrg: RGB565_16bit:[R7][R6][R5][R4][R3][G7][G6][G5]
// (CV_8UC2)            [G4][G3][G2][B7][B6][B5][B4][B3]
void BGR8882RGB565(Mat &imgSrc, Mat &imgTrg)
{
 uint8_t *ptrS;
 uint8_t *ptrT;

 if((imgSrc.cols != imgTrg.cols) || (imgSrc.rows != imgTrg.rows)){ // 確認輸入和輸出影像尺寸要相符
//     QMessageBox::critical(this,"Error","Image size different!");
     return;
 }

 for(int i=0; i<imgSrc.rows; i++){       // 設定迴圈數為影像高度
     ptrS = imgSrc.ptr<uint8_t>(i);      // 取得來源影像第i列(row)起始位置指標
     ptrT = imgTrg.ptr<uint8_t>(i);      // 取得目標影像第i列(row)起始位置指標

     for(int j=0,posS=0,posT=0; j<imgSrc.cols; j++,posS+=3, posT+=2){
         ptrT[posT]   = (ptrS[posS+2] & 0xF8)|         // RGB565高位元組(R5)
                        (ptrS[posS+1] >> 5);           // RGB565高位元組(G3)
         ptrT[posT+1] = ((ptrS[posS+1] & 0xF8) << 3) | // RGB565高/低位元組(G6)
                         (ptrS[posS] >> 3);            // RGB565低位元組B5)
     }
 }
}

void PanoConvert(Mat &imgSrc, double theta_v, int lcd_w, int lcd_h,
                 int  lcd_a, Mat &imgPano)
{
    int imgS_w = imgSrc.cols;
    int imgS_h = imgSrc.rows;
    int half_sh = imgS_h / 2;
    double vr = imgSrc.rows / CV_PI;
    double vw = vr * cos(theta_v / 2);
    double vh = vr * sin(theta_v / 2);
    int half_lcd_h = lcd_h / 2;
    double vk = vh / half_lcd_h;
    double vt = imgS_w / (double)(lcd_w * lcd_a);

    uint8_t *ptrS;
    uint8_t *ptrT;
    double theta;
    int sh, posS, posT;

    imgPano = Mat::zeros(Size(lcd_w*lcd_a, lcd_h), CV_8UC3);

    for(int i=0; i<lcd_h; i++){
        theta = atan(((half_lcd_h-i)*vk)/vw);
        sh = half_sh - vr * theta;
        ptrS = imgSrc.ptr<uint8_t>(sh);
        ptrT = imgPano.ptr<uint8_t>(i);

        for(int j=0; j<lcd_w*lcd_a; j++){
            posS = ((int)(j * vt)) * 3;
            posT = j * 3;
            ptrT[posT]   = ptrS[posS];
            ptrT[posT+1] = ptrS[posS+1];
            ptrT[posT+2] = ptrS[posS+2];
        }
    }
}
