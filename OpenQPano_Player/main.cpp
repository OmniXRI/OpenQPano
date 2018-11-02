#include <QCoreApplication>

#include <main.h>
#include <ili9225.h>

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
     cout << "Source and target image size mismatch !" << endl;
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

// must be run at sudo mode
int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    int lcd_state = LCD_Init(PIN_LCD_CSA);

    if(lcd_state!=0){
        cout << "LCD inital fail !" << endl;
        exit(0);
    }
    else{
        ILI9225_LCD_Test(PIN_LCD_CSA);
    }

    Mat imgLCD, imgSrc;
    VideoCapture cap1;
    int cap_w, cap_h, cap_f;
    uint8_t cs_pin[6] = {PIN_LCD_CS1, PIN_LCD_CS2, PIN_LCD_CS3,
                         PIN_LCD_CS4, PIN_LCD_CS5, PIN_LCD_CS6};
    uint8_t *ptrS;
    uint8_t *ptrT;

    while(1){
        // Show single picture to 6 LCDs
        imgSrc = imread("QVGA_V.jpg",IMREAD_COLOR);

        if(!imgSrc.empty()) {                                         // 若影像不空
             if( (imgSrc.rows != LCD_HEIGHT) ||
                 (imgSrc.cols != LCD_WIDTH) ){
                 cv::resize(imgSrc, imgSrc, Size(LCD_WIDTH, LCD_HEIGHT), 0, 0, INTER_LINEAR);
                 cout << "Image will resize to 176x220." << endl;

                 imgLCD = Mat::zeros(Size(imgSrc.cols,imgSrc.rows),CV_8UC2);

                 BGR8882RGB565(imgSrc, imgLCD);
                 ILI9225_Show_Data(PIN_LCD_CSA, imgLCD.ptr<uint8_t>(0),
                                   0, 0, imgLCD.cols, imgLCD.rows);
                 sleep(3);
             }
             else{
                 cout << "QVGA_V.jpg open fail !" << endl;
                 LCD_Close();
                 exit(0);
             }
        }

        // Play single video to 6 LCDs
        cap1 = VideoCapture("LCD_3.avi");

        if(cap1.isOpened()) {
            cap_w = cap1.get(CV_CAP_PROP_FRAME_WIDTH);
            cap_h = cap1.get(CV_CAP_PROP_FRAME_HEIGHT);
            cap_f = cap1.get(CV_CAP_PROP_FRAME_COUNT);
            imgLCD = Mat::zeros(Size(cap_w,cap_h),CV_8UC2);

            while(1){
                cap1 >> imgSrc;

                if(imgSrc.empty())
                    break;

                BGR8882RGB565(imgSrc, imgLCD);

                ILI9225_Show_Data(PIN_LCD_CSA, imgLCD.ptr<uint8_t>(0),
                                  0, 0, imgLCD.cols, imgLCD.rows);

                // check PB1 press
                if(bcm2835_gpio_lev(PIN_PB1) == LOW){
                    bcm2835_gpio_write(PIN_LED1, HIGH);
                    usleep(100000); // 100ms
                    cout << "OK" << endl;
                    bcm2835_gpio_write(PIN_LED1, LOW);
                    LCD_Close();
                    exit(0);
                }
            }

            cap1.release();
        }
        else{
             cout << "LCD_3.avi open fail !" << endl;
             LCD_Close();
             exit(0);
        }

        // Play Pano Video to 6 LCDs
        VideoCapture cap6 = VideoCapture("pano.mp4");

        if(!cap6.isOpened()){
            cout << "pano.avi open fail !" << endl;
            LCD_Close();
            exit(0);
        }

        cap_w = cap6.get(CV_CAP_PROP_FRAME_WIDTH);
        cap_h = cap6.get(CV_CAP_PROP_FRAME_HEIGHT);
        cap_f = cap6.get(CV_CAP_PROP_FRAME_COUNT);        

        if((cap_w != LCD_WIDTH*LCD_AMOUNT) || (cap_h != LCD_HEIGHT)){
            cout << "Video size error!" << endl;
            LCD_Close();
            exit(0);
        }

        Mat imgOri;//  = Mat::zeros(Size(LCD_WIDTH*LCD_AMOUNT,LCD_HEIGHT),CV_8UC3);
        Mat imgLCD6 = Mat::zeros(Size(LCD_WIDTH*LCD_AMOUNT,LCD_HEIGHT),CV_8UC2);
        Mat imgFrame = Mat::zeros(Size(LCD_WIDTH,LCD_HEIGHT),CV_8UC2);

        for(int f=0; f<cap_f; f++){
            cap6 >> imgOri;
            cout << "capture frame" << endl;

            if(imgOri.empty()){
                cout << "Frame import error!" << endl;
                LCD_Close();
                exit(0);
            }

            BGR8882RGB565(imgOri, imgLCD6);

            for(int n=0; n<LCD_AMOUNT; n++){
                for(int y=0; y<imgLCD6.rows; y++){
                    ptrS = imgLCD6.ptr<uint8_t>(y) + (n * LCD_WIDTH * 2);
                    ptrT = imgFrame.ptr<uint8_t>(y);

                    for(int x=0; x<LCD_WIDTH*2; x++){
                        ptrT[x] = ptrS[x];
                    }
                }

                ILI9225_Show_Data(cs_pin[n], imgFrame.ptr<uint8_t>(0),
                                  0, 0, imgFrame.cols, imgFrame.rows);

            }

            if(bcm2835_gpio_lev(PIN_PB1) == LOW){
                bcm2835_gpio_write(PIN_LED1, HIGH);
                usleep(100000); // 100ms
                cout << "OK" << endl;
                bcm2835_gpio_write(PIN_LED1, LOW);
                LCD_Close();
                exit(0);
            }
        }

        cap6.release();
    }
}
