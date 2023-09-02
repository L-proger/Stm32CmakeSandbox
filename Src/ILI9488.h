#pragma once

#include "PinMap.h"
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_spi.h>
#include <cstdint>
#include <FreeRTOS.h>
#include <task.h>

#define ILI9488_CMD_NOP                             0x00
#define ILI9488_CMD_SOFTWARE_RESET                  0x01
#define ILI9488_CMD_READ_DISP_ID                    0x04
#define ILI9488_CMD_READ_ERROR_DSI                  0x05
#define ILI9488_CMD_READ_DISP_STATUS                0x09
#define ILI9488_CMD_READ_DISP_POWER_MODE            0x0A
#define ILI9488_CMD_READ_DISP_MADCTRL               0x0B
#define ILI9488_CMD_READ_DISP_PIXEL_FORMAT          0x0C
#define ILI9488_CMD_READ_DISP_IMAGE_MODE            0x0D
#define ILI9488_CMD_READ_DISP_SIGNAL_MODE           0x0E
#define ILI9488_CMD_READ_DISP_SELF_DIAGNOSTIC       0x0F
#define ILI9488_CMD_ENTER_SLEEP_MODE                0x10
#define ILI9488_CMD_SLEEP_OUT                       0x11
#define ILI9488_CMD_PARTIAL_MODE_ON                 0x12
#define ILI9488_CMD_NORMAL_DISP_MODE_ON             0x13
#define ILI9488_CMD_DISP_INVERSION_OFF              0x20
#define ILI9488_CMD_DISP_INVERSION_ON               0x21
#define ILI9488_CMD_PIXEL_OFF                       0x22
#define ILI9488_CMD_PIXEL_ON                        0x23
#define ILI9488_CMD_DISPLAY_OFF                     0x28
#define ILI9488_CMD_DISPLAY_ON                      0x29
#define ILI9488_CMD_COLUMN_ADDRESS_SET              0x2A
#define ILI9488_CMD_MEMORY_WRITE                    0x2C
#define ILI9488_CMD_PAGE_ADDRESS_SET                0x2B
#define ILI9488_CMD_MEMORY_READ                     0x2E
#define ILI9488_CMD_PARTIAL_AREA                    0x30
#define ILI9488_CMD_VERT_SCROLL_DEFINITION          0x33
#define ILI9488_CMD_TEARING_EFFECT_LINE_OFF         0x34
#define ILI9488_CMD_TEARING_EFFECT_LINE_ON          0x35
#define ILI9488_CMD_MEMORY_ACCESS_CONTROL           0x36
#define ILI9488_CMD_VERT_SCROLL_START_ADDRESS       0x37
#define ILI9488_CMD_IDLE_MODE_OFF                   0x38
#define ILI9488_CMD_IDLE_MODE_ON                    0x39
#define ILI9488_CMD_COLMOD_PIXEL_FORMAT_SET         0x3A
#define ILI9488_CMD_WRITE_MEMORY_CONTINUE           0x3C
#define ILI9488_CMD_READ_MEMORY_CONTINUE            0x3E
#define ILI9488_CMD_SET_TEAR_SCANLINE               0x44
#define ILI9488_CMD_GET_SCANLINE                    0x45
#define ILI9488_CMD_WRITE_DISPLAY_BRIGHTNESS        0x51
#define ILI9488_CMD_READ_DISPLAY_BRIGHTNESS         0x52
#define ILI9488_CMD_WRITE_CTRL_DISPLAY              0x53
#define ILI9488_CMD_READ_CTRL_DISPLAY               0x54
#define ILI9488_CMD_WRITE_CONTENT_ADAPT_BRIGHTNESS  0x55
#define ILI9488_CMD_READ_CONTENT_ADAPT_BRIGHTNESS   0x56
#define ILI9488_CMD_WRITE_MIN_CAB_LEVEL             0x5E
#define ILI9488_CMD_READ_MIN_CAB_LEVEL              0x5F
#define ILI9488_CMD_READ_ABC_SELF_DIAG_RES          0x68
#define ILI9488_CMD_READ_ID1                        0xDA
#define ILI9488_CMD_READ_ID2                        0xDB
#define ILI9488_CMD_READ_ID3                        0xDC

/* Level 2 Commands (from the display Datasheet) */
#define ILI9488_CMD_INTERFACE_MODE_CONTROL          0xB0
#define ILI9488_CMD_FRAME_RATE_CONTROL_NORMAL       0xB1
#define ILI9488_CMD_FRAME_RATE_CONTROL_IDLE_8COLOR  0xB2
#define ILI9488_CMD_FRAME_RATE_CONTROL_PARTIAL      0xB3
#define ILI9488_CMD_DISPLAY_INVERSION_CONTROL       0xB4
#define ILI9488_CMD_BLANKING_PORCH_CONTROL          0xB5
#define ILI9488_CMD_DISPLAY_FUNCTION_CONTROL        0xB6
#define ILI9488_CMD_ENTRY_MODE_SET                  0xB7
#define ILI9488_CMD_BACKLIGHT_CONTROL_1             0xB9
#define ILI9488_CMD_BACKLIGHT_CONTROL_2             0xBA
#define ILI9488_CMD_HS_LANES_CONTROL                0xBE
#define ILI9488_CMD_POWER_CONTROL_1                 0xC0
#define ILI9488_CMD_POWER_CONTROL_2                 0xC1
#define ILI9488_CMD_POWER_CONTROL_NORMAL_3          0xC2
#define ILI9488_CMD_POWER_CONTROL_IDEL_4            0xC3
#define ILI9488_CMD_POWER_CONTROL_PARTIAL_5         0xC4
#define ILI9488_CMD_VCOM_CONTROL_1                  0xC5
#define ILI9488_CMD_CABC_CONTROL_1                  0xC6
#define ILI9488_CMD_CABC_CONTROL_2                  0xC8
#define ILI9488_CMD_CABC_CONTROL_3                  0xC9
#define ILI9488_CMD_CABC_CONTROL_4                  0xCA
#define ILI9488_CMD_CABC_CONTROL_5                  0xCB
#define ILI9488_CMD_CABC_CONTROL_6                  0xCC
#define ILI9488_CMD_CABC_CONTROL_7                  0xCD
#define ILI9488_CMD_CABC_CONTROL_8                  0xCE
#define ILI9488_CMD_CABC_CONTROL_9                  0xCF
#define ILI9488_CMD_NVMEM_WRITE                     0xD0
#define ILI9488_CMD_NVMEM_PROTECTION_KEY            0xD1
#define ILI9488_CMD_NVMEM_STATUS_READ               0xD2
#define ILI9488_CMD_READ_ID4                        0xD3
#define ILI9488_CMD_ADJUST_CONTROL_1                0xD7
#define ILI9488_CMD_READ_ID_VERSION                 0xD8
#define ILI9488_CMD_POSITIVE_GAMMA_CORRECTION       0xE0
#define ILI9488_CMD_NEGATIVE_GAMMA_CORRECTION       0xE1
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_1         0xE2
#define ILI9488_CMD_DIGITAL_GAMMA_CONTROL_2         0xE3
#define ILI9488_CMD_SET_IMAGE_FUNCTION              0xE9
#define ILI9488_CMD_ADJUST_CONTROL_2                0xF2
#define ILI9488_CMD_ADJUST_CONTROL_3                0xF7
#define ILI9488_CMD_ADJUST_CONTROL_4                0xF8
#define ILI9488_CMD_ADJUST_CONTROL_5                0xF9
#define ILI9488_CMD_SPI_READ_SETTINGS               0xFB
#define ILI9488_CMD_ADJUST_CONTROL_6                0xFC
#define ILI9488_CMD_ADJUST_CONTROL_7                0xFF


#define ILI9488_RED         0xF800

class ILI9488 {
public:

    ILI9488() {
        deselectChip();
        disableHwReset();
        enableHwReset();
        disableHwReset();


        auto v = read8(ILI9488_CMD_READ_ID3, 1);
        auto v2 = read8(ILI9488_CMD_READ_ID1, 1);
        auto v1 = read8(ILI9488_CMD_READ_ID2, 1);

        auto v3 = read8(ILI9488_CMD_READ_DISP_PIXEL_FORMAT, 1);



        spiBegin();
        writecommand(0xE0);
	    writedata(0x00);
	    writedata(0x03);
	    writedata(0x09);
	    writedata(0x08);
	    writedata(0x16);
	    writedata(0x0A);
	    writedata(0x3F);
	    writedata(0x78);
	    writedata(0x4C);
	    writedata(0x09);
	    writedata(0x0A);
	    writedata(0x08);
	    writedata(0x16);
	    writedata(0x1A);
	    writedata(0x0F);


	    writecommand(0XE1);
	    writedata(0x00);
	    writedata(0x16);
	    writedata(0x19);
	    writedata(0x03);
	    writedata(0x0F);
	    writedata(0x05);
	    writedata(0x32);
	    writedata(0x45);
	    writedata(0x46);
	    writedata(0x04);
	    writedata(0x0E);
	    writedata(0x0D);
	    writedata(0x35);
	    writedata(0x37);
	    writedata(0x0F);



	    writecommand(0XC0);      //Power Control 1
	    writedata(0x17);    //Vreg1out
	    writedata(0x15);    //Verg2out

	    writecommand(0xC1);      //Power Control 2
	    writedata(0x41);    //VGH,VGL

	    writecommand(0xC5);      //Power Control 3
	    writedata(0x00);
	    writedata(0x12);    //Vcom
	    writedata(0x80);

	    writecommand(0x36);      //Memory Access
	    writedata(0x48);

	    writecommand(0x3A);      // Interface Pixel Format
	    writedata(0x66); 	  //18 bit

	    writecommand(0XB0);      // Interface Mode Control
	    writedata(0x80);     			 //SDO NOT USE

	    writecommand(0xB1);      //Frame rate
	    writedata(0xA0);    //60Hz

	    writecommand(0xB4);      //Display Inversion Control
	    writedata(0x02);    //2-dot

       

        

	    writecommand(0XB6);      //Display Function Control  RGB/MCU Interface Control

	    writedata(0x02);    //MCU
	    writedata(0x02);    //Source,Gate scan dieection

	    writecommand(0XE9);      // Set Image Functio
	    writedata(0x00);    // Disable 24 bit data

	    writecommand(0xF7);      // Adjust Control
	    writedata(0xA9);
	    writedata(0x51);
	    writedata(0x2C);
	    writedata(0x82);    // D7 stream, loose


        writecommand(ILI9488_CMD_SLEEP_OUT);    //Exit Sleep
        spiEnd();


        vTaskDelay(120);

        spiBegin();
        writecommand(ILI9488_CMD_DISPLAY_ON);    //Display on
        spiEnd();
    }

    void enableHwReset() {
        LL_GPIO_ResetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
    }

     void disableHwReset() {
        LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port, LCD_RST_Pin);
    }


    void spiBegin() {
        while (LL_SPI_IsActiveFlag_BSY(SPI3));
        selectChip();
    }

    void spiEnd() {
        deselectChip();
    }


    std::uint8_t spiReadWrite8(std::uint8_t data) {
        while (!LL_SPI_IsActiveFlag_TXE(SPI3));
        LL_SPI_TransmitData8(SPI3, data);
       
        while(!LL_SPI_IsActiveFlag_RXNE(SPI3));
        return LL_SPI_ReceiveData8(SPI3);
    }


    void setInversionEnabled(bool enabled) {
        if(enabled){
            writecommand(ILI9488_CMD_DISP_INVERSION_ON);
        }else {
            writecommand(ILI9488_CMD_DISP_INVERSION_OFF);
        }
    }

    void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

        writecommand(ILI9488_CMD_COLUMN_ADDRESS_SET); // Column addr set
        writedata(x0 >> 8);
        writedata(x0 & 0xFF);     // XSTART
        writedata(x1 >> 8);
        writedata(x1 & 0xFF);     // XEND

        writecommand(ILI9488_CMD_PAGE_ADDRESS_SET); // Row addr set
        writedata(y0>>8);
        writedata(y0 &0xff);     // YSTART
        writedata(y1>>8);
        writedata(y1 &0xff);     // YEND

        writecommand(ILI9488_CMD_MEMORY_WRITE); // write to RAM

    }



    void write18BitColor(uint32_t color){
        // #if (__STM32F1__)
        //     uint8_t buff[4] = {
        //       (((color & 0xF800) >> 11)* 255) / 31,
        //       (((color & 0x07E0) >> 5) * 255) / 63,
        //       ((color & 0x001F)* 255) / 31
        //     };
        //     SPI.dmaSend(buff, 3);
        // #else
        uint8_t r = (color >> 12) & 0x3f;
        uint8_t g = (color >> 6) & 0x3f;
        uint8_t b = (color >> 0) & 0x3f;


       spiReadWrite8(r << 2);
       spiReadWrite8(g << 2);
       spiReadWrite8(b << 2);
    }


    void fillScreen(std::uint32_t color) {
        setAddrWindow(0,0, 320  -1 , 480 - 1);
        vTaskDelay(10);
        setDataMode();
         vTaskDelay(10);
        spiBegin();
 vTaskDelay(10);
        for(int i = 0; i < 320*480 / 2; ++i){
            write18BitColor(color);
        }

        spiEnd();

    }

    void writecommand(std::uint8_t command) {
        setCommandMode();
        selectChip();
        spiReadWrite8(command);
        deselectChip();
    }

     void writedata(std::uint8_t command) {
        setDataMode();
        selectChip();
        spiReadWrite8(command);
        deselectChip();
    }

    void read(uint8_t cmd, uint8_t* buffer, std::uint32_t len) {
        setCommandMode();
        spiBegin();
        spiReadWrite8(cmd);

        setDataMode();
        for(uint32_t i = 0; i < len; ++i){
            buffer[i] = spiReadWrite8(0x00);
        }

        spiEnd();
    }

    std::uint16_t read8(uint8_t cmd, uint8_t index) {
        spiBegin();
        setCommandMode();
        spiReadWrite8(0xD9);
        setDataMode();
        spiReadWrite8(0x10 + index);
        spiEnd();

        setCommandMode();
        spiBegin();

        spiReadWrite8(cmd);
        setDataMode();
        auto result =  spiReadWrite8(0x00);
        spiEnd();

        return result;
    }

  
    void setDataMode() {
        LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
    }

    void setCommandMode() {
        LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin);
    }

    void selectChip() {
        LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
    }

    void deselectChip() {
        LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin);
    }

};