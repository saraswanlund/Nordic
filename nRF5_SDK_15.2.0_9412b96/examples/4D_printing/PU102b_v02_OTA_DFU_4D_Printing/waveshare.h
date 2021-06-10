#ifndef WAVESHARE_H__
#define WAVESHARE_H__

#define WS_CS             26
#define WS_MISO           4
#define WS_MOSI           2
#define WS_SCK            3

#define WS_BUSY           29
#define WS_RST            28
#define WS_DC             27

#define EPD_WIDTH         104
#define EPD_HEIGHT        212

extern void WS_SetFrameMemory(unsigned char * image_buffer);
extern void WS_SetFullReg(void);
extern void WS_DisplayFrame(void);
extern void WS_ClearFrameMemory(void);
extern void WS_WaitUntilIdle(void);
extern void WS_SPI_init(void);
extern void WS_Reset(void);

#endif