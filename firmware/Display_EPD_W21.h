#ifndef _DISPLAY_EPD_W21_H_
#define _DISPLAY_EPD_W21_H_

#define EPD_WIDTH 800
#define EPD_HEIGHT 480
#define EPD_ARRAY EPD_WIDTH * EPD_HEIGHT / 8

// Full screen refresh display
void EPD_Full_Init(void);
void EPD_Full_Set_White_Basemap(void);

// Partial refresh display
void EPD_Part_Init(void);
void EPD_Part_Set_Data(unsigned int x_start, unsigned int y_start,
                       const unsigned char *datas, unsigned int PART_COLUMN,
                       unsigned int PART_LINE);
void EPD_Part_Set_White(unsigned int x_start, unsigned int y_start,
                        unsigned int PART_COLUMN, unsigned int PART_LINE);
void EPD_Part_Set_Black(unsigned int x_start, unsigned int y_start,
                        unsigned int PART_COLUMN, unsigned int PART_LINE);

// Shared sleep function
void EPD_DeepSleep(void);

#endif