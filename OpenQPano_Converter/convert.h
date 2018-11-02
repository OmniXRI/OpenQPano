#ifndef CONVERT_H
#define CONVERT_H

#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QDebug>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void ShowFrame(Mat imgS, QLabel *imgT);
void BGR8882RGB565(Mat &imgSrc, Mat &imgTrg);
void PanoConvert(Mat &imgSrc, double theta_v, int lcd_w, int lcd_h,
                 int  lcd_a, Mat &imgPano);

#endif // CONVERT_H
