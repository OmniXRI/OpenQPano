#include <bcm2835.h>
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PAGE_SIZE  (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

/* GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y) */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
#define GPIO_SET *(gpio+7)   /* sets   bits which are 1 ignores bits which are 0 */
#define GPIO_CLR *(gpio+10)  /* clears bits which are 1 ignores bits which are 0 */

#define LCD_WIDTH 176
#define LCD_HEIGHT 220
#define LCD_AMOUNT 6

// LCD GPIO define
#define PIN_LCD_RS   RPI_V2_GPIO_P1_07 // GPIO04, register select
#define PIN_LCD_WR   RPI_V2_GPIO_P1_29 // GPIO05, write
//#define PIN_LCD_RD                   //         read (reserver)
#define PIN_LCD_RST  RPI_V2_GPIO_P1_31 // GPIO06, reset
#define PIN_LCD_BL   RPI_V2_GPIO_P1_26 // GPIO07, backlight (LCD)
#define PIN_LCD_DB0  RPI_V2_GPIO_P1_38 // GPIO20, data bus bit 0
#define PIN_LCD_DB1  RPI_V2_GPIO_P1_40 // GPIO21, data bus bit 1
#define PIN_LCD_DB2  RPI_V2_GPIO_P1_15 // GPIO22, data bus bit 2
#define PIN_LCD_DB3  RPI_V2_GPIO_P1_16 // GPIO23, data bus bit 3
#define PIN_LCD_DB4  RPI_V2_GPIO_P1_18 // GPIO24, data bus bit 4
#define PIN_LCD_DB5  RPI_V2_GPIO_P1_22 // GPIO25, data bus bit 5
#define PIN_LCD_DB6  RPI_V2_GPIO_P1_37 // GPIO26, data bus bit 6
#define PIN_LCD_DB7  RPI_V2_GPIO_P1_13 // GPIO27, data bus bit 7

#define PIN_LCD_CS1  RPI_V2_GPIO_P1_32 // GPIO12, chip select 1
#define PIN_LCD_CS2  RPI_V2_GPIO_P1_33 // GPIO13, chip select 2
#define PIN_LCD_CS3  RPI_V2_GPIO_P1_36 // GPIO16, chip select 3
#define PIN_LCD_CS4  RPI_V2_GPIO_P1_11 // GPIO17, chip select 4
#define PIN_LCD_CS5  RPI_V2_GPIO_P1_12 // GPIO18, chip select 5
#define PIN_LCD_CS6  RPI_V2_GPIO_P1_35 // GPIO19, chip select 6
#define PIN_LCD_CSA  0                 // chip select 1 ~ 6

#define PIN_LED1     RPI_V2_GPIO_P1_08 // GPIO14, UART Tx, LED1
#define PIN_PB1      RPI_V2_GPIO_P1_10 // GPIO15, UART Rx, Push Button 1

// LCD常用色定義 (16bit, RGB565, 65536色)
#define VGA_BLACK    0x0000  // 黑色
#define VGA_WHITE    0xFFFF  // 白色
#define VGA_RED      0xF800  // 紅色
#define VGA_GREEN    0x0400  // 綠色
#define VGA_BLUE     0x001F  // 藍色
#define VGA_SILVER   0xC618  // 銀(亮灰)色
#define VGA_GRAY     0x8410  // 灰色
#define VGA_MAROON   0x8000  // 栗(暗紅)色
#define VGA_YELLOW   0xFFE0  // 黃色
#define VGA_OLIVE    0x8400  // 橄欖(暗綠)色
#define VGA_LIME     0x07E0  // 青檸(亮綠)色
#define VGA_AQUA     0x07FF  // 水藍(亮藍)色
#define VGA_TEAL     0x0410  // 水鳥(暗監綠)色
#define VGA_NAVY     0x0010  // 海軍(暗藍)色
#define VGA_FUCHSIA  0xF81F  // 紫紅色
#define VGA_PURPLE   0x8010  // 紫色

int  LCD_Init(uint8_t cs_num);
void LCD_Close(void);
void LCD_Backlight(uint8_t state);

void ILI9225_Delay_Loop(uint8_t num);
void ILI9225_CS(uint8_t cs_num, uint8_t state);
void ILI9225_Init(uint8_t cs_num);
void ILI9225_SetXY(uint8_t cs_num, int x1, int y1, int x2, int y2, int sx, int sy);
void ILI9225_WR_Cmd(char VL);
void ILI9225_WR_Data16(char VH, char VL);
void ILI9225_WR_Data8(char VL);
void ILI9225_WR_Cmd_Data(uint8_t cs_num, char cmd, int data);
void ILI9225_Clr_Screen(uint8_t cs_num);
void ILI9225_LCD_Test(uint8_t cs_num);
void ILI9225_Show_Data(uint8_t cs_num, uint8_t* pData, uint8_t sx, uint8_t sy, uint8_t img_w, uint8_t img_h);
void ILI9225_IO_Test(uint8_t pin_name, uint8_t state);
