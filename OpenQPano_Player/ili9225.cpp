//*************************************************************************************
// LCD 模塊驅動程式
// 176*220像素，65,536(2^16)色(RGB565)，8 bit資料匯流排, ILI9225驅動晶片
// 完整資訊可參考http://www.displayfuture.com/Display/datasheet/controller/ILI9225.pdf
//*************************************************************************************

#include "ili9225.h"

struct timespec sleepTime;
struct timespec returnTime;

int  mem_fd;
void *gpio_map;
volatile unsigned *gpio;

// Set up a memory regions to access GPIO
void setup_io()
{
        /* open /dev/mem */
        if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
                printf("can't open /dev/mem \n");
                exit(-1);
        }

        /* mmap GPIO */
        gpio_map = mmap(
                NULL,             /* Any adddress in our space will do */
                BLOCK_SIZE,       /* Map length */
                PROT_READ|PROT_WRITE, /* Enable reading & writting to mapped memory */
                MAP_SHARED,       /* Shared with other processes */
                mem_fd,           /* File to map */
                GPIO_BASE         /* Offset to GPIO peripheral */
                );

        close(mem_fd); /* No need to keep mem_fd open after mmap */

        if (gpio_map == MAP_FAILED) {
                printf("mmap error %d\n", (int)gpio_map); /* errno also set! */
                exit(-1);
        }

        /* Always use volatile pointer! */
        gpio = (volatile unsigned *)gpio_map;
}

// LCD module inital
int LCD_Init(uint8_t cs_num)
{
 if(!bcm2835_init()){ //若BCM2835庫初始化失敗則返回錯誤碼1
   return 1;
 }

 setup_io(); // Set up gpi pointer for direct register access

 bcm2835_gpio_fsel(PIN_LCD_RS,  BCM2835_GPIO_FSEL_OUTP);
// bcm2835_gpio_fsel(PIN_LCD_WR,  BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_RST, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_BL,  BCM2835_GPIO_FSEL_OUTP);

 bcm2835_gpio_write(PIN_LCD_RS,  HIGH);
// bcm2835_gpio_write(PIN_LCD_WR,  HIGH);
 bcm2835_gpio_write(PIN_LCD_RST, HIGH);
 bcm2835_gpio_write(PIN_LCD_BL,  HIGH);

 OUT_GPIO(5);       // set WR (GPIO5) for output port
 GPIO_SET = 1 << 5; // set WR (GPIO5)

 bcm2835_gpio_fsel(PIN_LCD_DB0, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB1, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB2, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB3, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB4, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB5, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB6, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_DB7, BCM2835_GPIO_FSEL_OUTP);

 bcm2835_gpio_fsel(PIN_LCD_CS1, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_CS2, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_CS3, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_CS4, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_CS5, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_LCD_CS6, BCM2835_GPIO_FSEL_OUTP);

 bcm2835_gpio_write(PIN_LCD_CS1, HIGH);
 bcm2835_gpio_write(PIN_LCD_CS2, HIGH);
 bcm2835_gpio_write(PIN_LCD_CS3, HIGH);
 bcm2835_gpio_write(PIN_LCD_CS4, HIGH);
 bcm2835_gpio_write(PIN_LCD_CS5, HIGH);
 bcm2835_gpio_write(PIN_LCD_CS6, HIGH);

 bcm2835_gpio_fsel(PIN_LED1, BCM2835_GPIO_FSEL_OUTP);
 bcm2835_gpio_fsel(PIN_PB1, BCM2835_GPIO_FSEL_INPT);
 bcm2835_gpio_write(PIN_LED1, LOW);

 LCD_Backlight(HIGH); //點亮LCD背光
 ILI9225_Init(cs_num);
 ILI9225_Clr_Screen(cs_num);

 sleepTime.tv_sec = 0;
 sleepTime.tv_nsec = 500;

 return 0;
}

void LCD_Close()
{
 LCD_Backlight(LOW); //關閉LCD背光
 bcm2835_close();
}

// 設定LCD背光 0(LOW)關閉, 1(HIGH)開啟
void LCD_Backlight(uint8_t state)
{
 bcm2835_gpio_write(PIN_LCD_BL, state);
}

void ILI9225_Delay_Loop(uint8_t num)
{
 for(int i=0; i<num; i++){
     // nothing to do
 }
}

void ILI9225_CS(uint8_t cs_num, uint8_t state)
{
    if(cs_num == PIN_LCD_CS1 || cs_num == PIN_LCD_CS2 || cs_num == PIN_LCD_CS3 ||
       cs_num == PIN_LCD_CS4 || cs_num == PIN_LCD_CS5 || cs_num == PIN_LCD_CS6 ){
       bcm2835_gpio_write(cs_num, state);
    }
    else{
       bcm2835_gpio_write(PIN_LCD_CS1, state);
       bcm2835_gpio_write(PIN_LCD_CS2, state);
       bcm2835_gpio_write(PIN_LCD_CS3, state);
       bcm2835_gpio_write(PIN_LCD_CS4, state);
       bcm2835_gpio_write(PIN_LCD_CS5, state);
       bcm2835_gpio_write(PIN_LCD_CS6, state);
    }

//    ILI9225_Delay_Loop(2);
}

// LCD初始化
// 【注意】不熟悉參數設定的人請勿任意調整下列參數，以免造成LCD損壞或無法正常顯示
void ILI9225_Init(uint8_t cs_num)
{
 // Reset LCD
 bcm2835_gpio_write(PIN_LCD_RST, HIGH);
 delay(1);
 bcm2835_gpio_write(PIN_LCD_RST, LOW);
 delay(10);
 bcm2835_gpio_write(PIN_LCD_RST, HIGH);
 delay(50);

 ILI9225_WR_Cmd_Data(cs_num, 0x10, 0x0000); // Power Control 1
 ILI9225_WR_Cmd_Data(cs_num, 0x11, 0x0000); // Power Control 2
 ILI9225_WR_Cmd_Data(cs_num, 0x12, 0x0000); // Power Control 3
 ILI9225_WR_Cmd_Data(cs_num, 0x13, 0x0000); // Power Control 4
 ILI9225_WR_Cmd_Data(cs_num, 0x14, 0x0000); // Power Control 5
 delay(40);
 ILI9225_WR_Cmd_Data(cs_num, 0x11, 0x0018); // Power Control 2
 ILI9225_WR_Cmd_Data(cs_num, 0x12, 0x6121); // Power Control 3
 ILI9225_WR_Cmd_Data(cs_num, 0x13, 0x006F); // Power Control 4
 ILI9225_WR_Cmd_Data(cs_num, 0x14, 0x495F); // Power Control 5
 ILI9225_WR_Cmd_Data(cs_num, 0x10, 0x0800); // Power Control 1
 delay(10);
 ILI9225_WR_Cmd_Data(cs_num, 0x11, 0x103B); // Power Control 2
 delay(50);
 ILI9225_WR_Cmd_Data(cs_num, 0x01, 0x011C); // Driver Output Control
 ILI9225_WR_Cmd_Data(cs_num, 0x02, 0x0100); // LCD AC Driving Control
 ILI9225_WR_Cmd_Data(cs_num, 0x03, 0x1030); // Enter Mode
 ILI9225_WR_Cmd_Data(cs_num, 0x07, 0x0000); // Display Control 1
 ILI9225_WR_Cmd_Data(cs_num, 0x08, 0x0808); // Blank Period Control 1
 ILI9225_WR_Cmd_Data(cs_num, 0x0b, 0x1100); // Frame Cycle Control
 ILI9225_WR_Cmd_Data(cs_num, 0x0c, 0x0000); // Interface Control
 ILI9225_WR_Cmd_Data(cs_num, 0x0f, 0x0D01); // Oscillation Control
 ILI9225_WR_Cmd_Data(cs_num, 0x15, 0x0020); // ??
 ILI9225_WR_Cmd_Data(cs_num, 0x20, 0x0000); // RAM Address Set 1
 ILI9225_WR_Cmd_Data(cs_num, 0x21, 0x0000); // RAM Address Set 2
 ILI9225_WR_Cmd_Data(cs_num, 0x30, 0x0000); // Gate Scan Control
 ILI9225_WR_Cmd_Data(cs_num, 0x31, 0x00DB); // Vertical Scroll Control 1
 ILI9225_WR_Cmd_Data(cs_num, 0x32, 0x0000); // Vertical Scroll Control 2
 ILI9225_WR_Cmd_Data(cs_num, 0x33, 0x0000); // Vertical Scroll Control 3
 ILI9225_WR_Cmd_Data(cs_num, 0x34, 0x00DB); // Partial Driving Position -1
 ILI9225_WR_Cmd_Data(cs_num, 0x35, 0x0000); // Partial Driving Position -2
 ILI9225_WR_Cmd_Data(cs_num, 0x36, 0x00AF); // Horizontal Window Address -1
 ILI9225_WR_Cmd_Data(cs_num, 0x37, 0x0000); // Horizontal Window Address -2
 ILI9225_WR_Cmd_Data(cs_num, 0x38, 0x00DB); // Vertical Window Address -1
 ILI9225_WR_Cmd_Data(cs_num, 0x39, 0x0000); // Vertical Window Address -2
 ILI9225_WR_Cmd_Data(cs_num, 0x50, 0x0000); // Gamma Control 1
 ILI9225_WR_Cmd_Data(cs_num, 0x51, 0x0808); // Gamma Control 2
 ILI9225_WR_Cmd_Data(cs_num, 0x52, 0x080A); // Gamma Control 3
 ILI9225_WR_Cmd_Data(cs_num, 0x53, 0x000A); // Gamma Control 4
 ILI9225_WR_Cmd_Data(cs_num, 0x54, 0x0A08); // Gamma Control 5
 ILI9225_WR_Cmd_Data(cs_num, 0x55, 0x0808); // Gamma Control 6
 ILI9225_WR_Cmd_Data(cs_num, 0x56, 0x0000); // Gamma Control 7
 ILI9225_WR_Cmd_Data(cs_num, 0x57, 0x0A00); // Gamma Control 8
 ILI9225_WR_Cmd_Data(cs_num, 0x58, 0x0710); // Gamma Control 9
 ILI9225_WR_Cmd_Data(cs_num, 0x59, 0x0710); // Gamma Control 10
 ILI9225_WR_Cmd_Data(cs_num, 0x07, 0x0012); // Display Control 1
 delay(50);
 ILI9225_WR_Cmd_Data(cs_num, 0x07, 0x1017); // Display Control 1

// ILI9225_CS(cs_num, LOW);           // 晶片選擇線設為低電位
// ILI9225_WR_Cmd(0x22);              // Write Command to GRAM
// ILI9225_CS(cs_num, HIGH);          // 晶片選擇線設為高電位
}

// 設定LCD讀寫視窗範圍
// [in] x1,y1 為左上座標
// [in] x2,y2 為右下座標
// [in] sx,sy 為起始座標
// 必須符合 x1 < sx < x2, y1 < sy < y2,
//        x1 >= 0 && x2 < 176, y1 >= 0 && y2 < 220
// 座標是以直式顯示時表示，當橫式顯示時座標須逆時針轉90度
void ILI9225_SetXY(uint8_t cs_num, int x1, int y1, int x2, int y2, int sx, int sy)
{
 ILI9225_WR_Cmd_Data(cs_num, 0x36, x2); // HEX
 ILI9225_WR_Cmd_Data(cs_num, 0x37, x1); // HSX
 ILI9225_WR_Cmd_Data(cs_num, 0x38, y2); // VEY
 ILI9225_WR_Cmd_Data(cs_num, 0x39, y1); // VSY
 ILI9225_WR_Cmd_Data(cs_num, 0x20, sx); // GRAM start address low  byte
 ILI9225_WR_Cmd_Data(cs_num, 0x21, sy); // GRAM start address high byte

// ILI9225_CS(cs_num, LOW);       // 晶片選擇線設為低電位
// ILI9225_WR_Cmd(0x22);          // Write Command to GRAM
// ILI9225_CS(cs_num, HIGH);      // 晶片選擇線設為高電位
}

// LCD寫入控制命令
// [in] VL 只送低位元組，高位元組一律填0x00
void ILI9225_WR_Cmd(char VL)
{
 bcm2835_gpio_write(PIN_LCD_RS, LOW);  // Register選擇線設為低電位
 ILI9225_WR_Data16(0,VL);              // LCD寫入2個位元組(16 bit)資料
 bcm2835_gpio_write(PIN_LCD_RS, HIGH); // Register選擇線設為高電位
}

// LCD寫入2個位元組(16 bit)資料
// [in] VH 為資料高位元組
// [in] VL 為資料低位元組
void ILI9225_WR_Data16(char VH, char VL)
{
 ILI9225_WR_Data8(VH);         // LCD寫入1個(高)位元組資料
 ILI9225_WR_Data8(VL);         // LCD寫入1個(低)位元組資料
}

// LCD寫入1個位元組(8 bit)資料
// [in] VL 為資料
void ILI9225_WR_Data8(char VL)
{
// bcm2835_gpio_write(PIN_LCD_WR, LOW);  // Write線設為低電位
// ILI9225_Delay_Loop(2);
// nanosleep(&sleepTime, &returnTime);

//// uint32_t value = ((uint32_t)VL) << 20;   // LCD DB7~DB0 = GPIO27 ~ GPIO20
// uint32_t value = ((uint32_t)VL) * 1048576;   // 2^20, LCD DB7~DB0 = GPIO27 ~ GPIO20
// uint32_t mask = 0x0ff00000;

// bcm2835_gpio_write_mask(value,mask);
// ILI9225_Delay_Loop(0);
// nanosleep(&sleepTime, &returnTime);

// bcm2835_gpio_write(PIN_LCD_WR, HIGH);
// ILI9225_Delay_Loop(3);
// nanosleep(&sleepTime, &returnTime);

 GPIO_CLR = 32; // clear WR (GPIO5, 1<<5)
 GPIO_CLR = 0;
 GPIO_SET = ((uint32_t)VL) * 1048576;     // (VL<<20)  DB7~DB0=GPIO27~GPIO20
 GPIO_SET = 0;
 GPIO_CLR = ((uint32_t)(~VL)) * 1048576;  // (~VL<<20) DB7~DB0=GPIO27~GPIO20
 GPIO_CLR = 0;
 asm volatile ("nop");
 GPIO_SET = 32; // set WR (GPIO5, 1<<5)
 GPIO_SET = 0;
 asm volatile ("nop");
 asm volatile ("nop");
// asm volatile ("nop");
}

// LCD寫入一組完整命令(暫存器編號加16bit資料)
// [in] cmd  命令暫存器編號
// [in] data 資料
void ILI9225_WR_Cmd_Data(uint8_t cs_num, char cmd, int data)
{
  ILI9225_CS(cs_num, LOW);          // 晶片選擇線設為低電位
  ILI9225_WR_Cmd(cmd);              // 寫入命令
  ILI9225_WR_Data16(data>>8, data); // 寫入資料(高位元組，低位元組)
  ILI9225_CS(cs_num, HIGH);         // 晶片選擇線設為高電位
}

// LCD清除畫面(全部黑屏)
// 即將GRAM全部填入0x00
void ILI9225_Clr_Screen(uint8_t cs_num)
{
    if(cs_num == PIN_LCD_CS1 || cs_num == PIN_LCD_CS2 || cs_num == PIN_LCD_CS3 ||
       cs_num == PIN_LCD_CS4 || cs_num == PIN_LCD_CS5 || cs_num == PIN_LCD_CS6 ){
        ILI9225_SetXY(cs_num, 0, 0, 175, 219, 0, 0); // 設定視窗範圍為全螢幕
    }
    else{
        ILI9225_SetXY(PIN_LCD_CSA, 0, 0, 175, 219, 0, 0); // 設定視窗範圍為全螢幕
    }

    ILI9225_CS(cs_num, LOW);             // 晶片選擇線設為低電位
    ILI9225_WR_Cmd(0x22);                // Write Command to GRAM

    uint32_t value = 0;                  // LCD DB7~DB0 = GPIO27 ~ GPIO20
    uint32_t mask = 0x0ff00000;

    bcm2835_gpio_write_mask(value,mask); // Clear data bus

    for(int i=0; i<LCD_HEIGHT; i++){            // 設定迴圈數為圖框高度
        for(int j=0; j<LCD_WIDTH*2; j++){      // 設定迴圈數為圖框寬度乘2，每個點為2個位元組(RGB565)
//            bcm2835_gpio_write(PIN_LCD_WR, LOW);  // 設定LCD寫入線為低電位
//            bcm2835_gpio_write(PIN_LCD_WR, HIGH); // 設定LCD寫入線為高電位

            GPIO_CLR = 32; // clear WR (1<<5, GPIO5)
            GPIO_CLR = 0;
            asm volatile ("nop");
            GPIO_SET = 32; // set WR (GPIO5, 1<<5)
            GPIO_SET = 0;
            asm volatile ("nop");
        }

//        ILI9225_Delay_Loop(2);
//        nanosleep(&sleepTime, &returnTime);
    }

    ILI9225_CS(cs_num, HIGH);            // 晶片選擇線設為高電位
}

// LCD測試
// 顯示十六色橫條紋於畫面上
void ILI9225_LCD_Test(uint8_t cs_num)
{
    int color_table[16] = {VGA_BLACK, VGA_WHITE, VGA_RED, VGA_GREEN,
                           VGA_BLUE,  VGA_SILVER, VGA_GRAY, VGA_MAROON,
                           VGA_YELLOW, VGA_OLIVE, VGA_LIME, VGA_AQUA,
                           VGA_TEAL, VGA_NAVY, VGA_FUCHSIA, VGA_PURPLE}; // 色彩索引表

    if(cs_num == PIN_LCD_CS1 || cs_num == PIN_LCD_CS2 || cs_num == PIN_LCD_CS3 ||
       cs_num == PIN_LCD_CS4 || cs_num == PIN_LCD_CS5 || cs_num == PIN_LCD_CS6 ){
        ILI9225_SetXY(cs_num, 0, 0, 175, 219, 0, 0);      // 設定視窗範圍為全螢幕
    }
    else{
        ILI9225_SetXY(PIN_LCD_CSA, 0, 0, 175, 219, 0, 0); // 設定視窗範圍為全螢幕
    }

    ILI9225_CS(cs_num, LOW);       // 晶片選擇線設為低電位
    ILI9225_WR_Cmd(0x22);          // Write Command to GRAM

    int index, color, offset;

    for(int y=0; y<LCD_HEIGHT; y++){               // 設定迴圈數為圖框高度
        index = (y/10) % 16;              // 取得顏色表索引值
      for(int x=0; x<LCD_WIDTH; x++){             // 設定迴圈數為圖框寬度，每個點為2個位元組(RGB565)
        offset = ((x/11) + index) % 16;
        color = color_table[offset];       // 取得欲寫入顏色內容
        ILI9225_WR_Data16(color>>8, color); // 寫入LCD GRAM中        
      }

//      ILI9225_Delay_Loop(2);
//      nanosleep(&sleepTime, &returnTime);
    }

    ILI9225_CS(cs_num, HIGH);       // 晶片選擇線設為高電位
}

void ILI9225_Show_Data(uint8_t cs_num, uint8_t* pData,
                       uint8_t sx, uint8_t sy, uint8_t img_w, uint8_t img_h)
{
    uint8_t ex = sx + img_w - 1;
    uint8_t ey = sy + img_h - 1;

    if(cs_num == PIN_LCD_CS1 || cs_num == PIN_LCD_CS2 || cs_num == PIN_LCD_CS3 ||
       cs_num == PIN_LCD_CS4 || cs_num == PIN_LCD_CS5 || cs_num == PIN_LCD_CS6 ){
        ILI9225_SetXY(cs_num, sx, sy, ex, ey, sx, sy);      // 設定視窗範圍
    }
    else{
        ILI9225_SetXY(PIN_LCD_CSA, sx, sy, ex, ey, sx, sy); // 設定視窗範圍
    }

    ILI9225_CS(cs_num, LOW);       // 晶片選擇線設為低電位
    ILI9225_WR_Cmd(0x22);          // Write Command to GRAM

    for(int y=0; y<=ey; y++){               // 設定迴圈數為圖框高度
      for(int x=0; x<=ex; x++){             // 設定迴圈數為圖框寬度，每個點為2個位元組(RGB565)
          ILI9225_WR_Data8(*(pData++)); // High Byte寫入LCD GRAM中
          asm volatile ("nop");
          asm volatile ("nop");
          asm volatile ("nop");
//          asm volatile ("nop");
//          asm volatile ("nop");
//          asm volatile ("nop");
          ILI9225_WR_Data8(*(pData++)); // Low Byte寫入LCD GRAM中
          asm volatile ("nop");
          asm volatile ("nop");
          asm volatile ("nop");
//          asm volatile ("nop");
//          asm volatile ("nop");
//          asm volatile ("nop");

//          ILI9225_WR_Data16(*(pData++), *(pData++)); // 寫入LCD GRAM中
      }

//      ILI9225_Delay_Loop(2);
//      nanosleep(&sleepTime, &returnTime);
    }

    ILI9225_CS(cs_num, HIGH);       // 晶片選擇線設為高電位
}

void ILI9225_IO_Test(uint8_t pin_name, uint8_t state)
{
    bcm2835_gpio_write(pin_name, state);
}

