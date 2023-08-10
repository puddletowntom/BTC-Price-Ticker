#pragma once

// RGB SPI interface
#define LCD_SPI_DATA0     39  /*!< for 1-line SPI, this also refered as MOSI */
#define LCD_SPI_CLK       38
#define LCD_SPI_CS        47
#define LCD_SPI_DC        -1
#define LCD_SPI_RST       40

// RGB interface
#define LCD_PCLK_GPIO     (48)
#define LCD_VSYNC_GPIO    (18)
#define LCD_HSYNC_GPIO    (17)
#define LCD_DE_GPIO       (21)

#define RGB_B0       (16)  // B0
#define RGB_B1       (15)  // B1
#define RGB_B2       (14)   // B2
#define RGB_B3       (13)  // B3
#define RGB_B4       (12)  // B4
#define RGB_G0       (11)   // G0
#define RGB_G1       (10)  // G1
#define RGB_G2       (9)  // G2
#define RGB_G3       (8)  // G3
#define RGB_G4       (7)  // G4
#define RGB_G5      (6)  // G5
#define RGB_R0      (5)  // R0
#define RGB_R1      (4)  // R1
#define RGB_R2      (3)   // R2
#define RGB_R3      (2)  // R3
#define RGB_R4      (1)  // R4

#define LCD_DATA0_GPIO    (RGB_B0)   // B0
#define LCD_DATA1_GPIO    (RGB_B1)   // B1
#define LCD_DATA2_GPIO    (RGB_B2)   // B2
#define LCD_DATA3_GPIO    (RGB_B3)   // B3
#define LCD_DATA4_GPIO    (RGB_B4)   // B4
#define LCD_DATA5_GPIO    (RGB_G0)   // G0
#define LCD_DATA6_GPIO    (RGB_G1)    // G1
#define LCD_DATA7_GPIO    (RGB_G2)   // G2
#define LCD_DATA8_GPIO    (RGB_G3)   // G3
#define LCD_DATA9_GPIO    (RGB_G4)   // G4
#define LCD_DATA10_GPIO   (RGB_G5)   // G5
#define LCD_DATA11_GPIO   (RGB_R0)   // R0
#define LCD_DATA12_GPIO   (RGB_R1)    // R1
#define LCD_DATA13_GPIO   (RGB_R2)    // R2
#define LCD_DATA14_GPIO   (RGB_R3)    // R3
#define LCD_DATA15_GPIO   (RGB_R4)    // R4
#define LCD_DISP_EN_GPIO  (-1)

#define LCD_PIN_BK_LIGHT       41

#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

//#define ENCODER_A_PIN     (5)
//#define ENCODER_B_PIN     (6)

//#define MOTOR_PIN         (7)
//#define LED_PIN           (4)

//#define BTN_PIN           (3)

//#define EXT_PIN_0         (1)
//#define EXT_PIN_1         (2)
//#define EXT_PIN_2         (20)
//#define EXT_PIN_3         (19)
