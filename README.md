# CH32V003 SSD1306 ULWOS2

## Simple countodown Timer SSD1306 OLED Using ULWOS2 scheduler 
I'm playng with this STM32 clones micros wit RISC-V core instead ARM. 
This is a simple countdown timer: with first switch button is possible start and pause timer with a short press. With long press time is resetted and restartd. With a long press on second switch button you can get in settings. change timeout minutes with short press. Long press again for exit from settings. Settings wil be stored in a portion of flash so data will be mantained on power off or reset. I'd also started to test ULWOS2 sheduler library and it works well with low resources MCU. CH32V003 has only 16K flash and 2K ram.
SSD1306 drivers supports Mikroe GLCD Font Creator.

References
1. https://github.com/fabiopjve/ULWOS2
2. https://www.mikroe.com/glcd-font-creator

Library supports C font array created by GLCD Font Creator by MikroElektronika.<br>
https://www.mikroe.com/glcd-font-creator<br>

Please refer to the following link to get more details about fonts.<br>
https://os.mbed.com/users/dreschpe/code/SPI_TFT_ILI9341/<br>

[ How to add new fonts ]
1. Run GLCD Font Creator
2. Click File-New Font-Import An Existing System Font
3. Select font, style and size from font dialog.
4. GLCD Font Cretor makes Bitmap fonts
5. Click Export for GLCD menu
6. Select mikroC tab.
7. Copy generated code to fonts.c file.
8. Modify data type from unsigned short to uint8_t
9. Add optional bytes (offset, width, height, bpl, staring char) to the array. open fonts.c for details.
10. Add extern declaration to fonts.h file.
