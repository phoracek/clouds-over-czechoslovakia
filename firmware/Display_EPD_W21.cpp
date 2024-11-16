#include "Display_EPD_W21.h"
#include "Display_EPD_W21_spi.h"

// Wait for the display to become idle.
void epd_chkstatus(void) {
  while (1) { // =0 BUSY
    if (isEPD_W21_BUSY == 1)
      break;
  }
}

// Full screen refresh.
void epd_update(void) {
  EPD_W21_WriteCMD(0x12); // Display refresh
  delay(1);               // The delay here is necessary, 200uS at least
  epd_chkstatus();
}

// Initialize the screen for the following update function.
void EPD_Full_Init(void) {
  EPD_W21_RST_0; // Module reset
  delay(10);     // At least 10ms delay
  EPD_W21_RST_1;
  delay(10);     // At least 10ms delay

  EPD_W21_WriteCMD(0x01);  // Power setting
  EPD_W21_WriteDATA(0x07);
  EPD_W21_WriteDATA(0x07); // VGH=20V,VGL=-20V
  EPD_W21_WriteDATA(0x3f); // VDH=15V
  EPD_W21_WriteDATA(0x3f); // VDL=-15V

  // Enhanced display drive
  EPD_W21_WriteCMD(0x06); // Booster Soft Start
  EPD_W21_WriteDATA(0x17);
  EPD_W21_WriteDATA(0x17);
  EPD_W21_WriteDATA(0x28);
  EPD_W21_WriteDATA(0x17);

  EPD_W21_WriteCMD(0x04); // Power on
  delay(100);
  epd_chkstatus();

  EPD_W21_WriteCMD(0X00);  // Panel setting
  EPD_W21_WriteDATA(0x1F); // KW-3f   KWR-2F BWROTP 0f BWOTP 1f

  EPD_W21_WriteCMD(0x61);  // Tres
  EPD_W21_WriteDATA(0x03); // Source 800
  EPD_W21_WriteDATA(0x20);
  EPD_W21_WriteDATA(0x01); // Gate 480
  EPD_W21_WriteDATA(0xE0);

  EPD_W21_WriteCMD(0X15);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0X50); // VCOM and data interval setting
  EPD_W21_WriteDATA(0x10);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0X60); // TCON setting
  EPD_W21_WriteDATA(0x22);
}

// Initialize the screen for the following partial update function.
void EPD_Part_Init(void) {
  EPD_W21_RST_0; // Module reset
  delay(10);     // At least 10ms delay
  EPD_W21_RST_1;
  delay(10);     // At least 10ms delay

  EPD_W21_WriteCMD(0X00);  // Panel setting
  EPD_W21_WriteDATA(0x1F); // KW-3f   KWR-2F BWROTP 0f BWOTP 1f

  EPD_W21_WriteCMD(0x04); // Power on
  delay(100);
  epd_chkstatus();

  EPD_W21_WriteCMD(0xE0);
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteCMD(0xE5);
  EPD_W21_WriteDATA(0x6E);
}

// Clear screen display.
//
// This should be called before starting partial refreshes of the display.
void EPD_Full_Set_White_Basemap(void) {
  unsigned int i;

  // Write Data
  EPD_W21_WriteCMD(0x10);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0xFF); // is  different
  }

  EPD_W21_WriteCMD(0x13);
  for (i = 0; i < EPD_ARRAY; i++) {
    EPD_W21_WriteDATA(0x00);
  }

  epd_update();
}

// Ready up the display for the following update call.
void epd_part_prepare_for_write(unsigned int x_start, unsigned int y_start,
                      unsigned int PART_COLUMN, unsigned int PART_LINE) {
  unsigned int x_end, y_end;

  x_end = x_start + PART_LINE - 1;
  y_end = y_start + PART_COLUMN - 1;

  EPD_W21_WriteCMD(0x50);
  EPD_W21_WriteDATA(0xA9);
  EPD_W21_WriteDATA(0x07);

  EPD_W21_WriteCMD(0x91); // This command makes the display enter partial mode
  EPD_W21_WriteCMD(0x90); // Resolution setting
  EPD_W21_WriteDATA(x_start / 256);
  EPD_W21_WriteDATA(x_start % 256); // x-start

  EPD_W21_WriteDATA(x_end / 256);
  EPD_W21_WriteDATA(x_end % 256 - 1); // x-end

  EPD_W21_WriteDATA(y_start / 256);
  EPD_W21_WriteDATA(y_start % 256); // y-start

  EPD_W21_WriteDATA(y_end / 256);
  EPD_W21_WriteDATA(y_end % 256 - 1); // y-end
  EPD_W21_WriteDATA(0x01);

  EPD_W21_WriteCMD(0x13); // Writes New data to SRAM
}

// Render the given data on the selected area.
//
// The given data must contain only the picture to be rendered, i.e. not the
// whole screen.
//
// Drawing on large portions of the screen increases change of glitches.
//
// Repeated partial updates add risk of ghosting. Cycling the given part through
// white and black decreases that risk.
void EPD_Part_Set_Data(unsigned int x_start, unsigned int y_start,
                       const unsigned char *datas, unsigned int PART_COLUMN,
                       unsigned int PART_LINE) {
  epd_part_prepare_for_write(x_start, y_start, PART_COLUMN, PART_LINE);

  unsigned int i;
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
    EPD_W21_WriteDATA(datas[i]);
  }

  epd_update();
}

// Make selected area black.
void EPD_Part_Set_Black(unsigned int x_start, unsigned int y_start,
                        unsigned int PART_COLUMN, unsigned int PART_LINE) {
  epd_part_prepare_for_write(x_start, y_start, PART_COLUMN, PART_LINE);

  unsigned int i;
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
    EPD_W21_WriteDATA(0x00);
  }

  epd_update();
}

// Make selected area white.
void EPD_Part_Set_White(unsigned int x_start, unsigned int y_start,
                        unsigned int PART_COLUMN, unsigned int PART_LINE) {
  epd_part_prepare_for_write(x_start, y_start, PART_COLUMN, PART_LINE);

  unsigned int i;
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++) {
    EPD_W21_WriteDATA(0xFF);
  }

  epd_update();
}

// Deep sleep function.
//
// This must be called after update is complated. Otherwise the
// display may get damaged.
void EPD_DeepSleep(void) {
  EPD_W21_WriteCMD(0X50);  // VCOM and data interval setting
  EPD_W21_WriteDATA(0xf7); // WBmode:VBDF 17|D7 VBDW 97 VBDB 57    WBRmode:VBDF
                           // F7 VBDW 77 VBDB 37  VBDR B7

  EPD_W21_WriteCMD(0X02); // Power off
  epd_chkstatus();
  EPD_W21_WriteCMD(0X07); // Deep sleep
  EPD_W21_WriteDATA(0xA5);
}