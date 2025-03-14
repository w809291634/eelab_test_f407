#ifndef _DCMI_H
#define _DCMI_H
#include "extend.h"   									
#include "gui.h"

extern volatile u8 ov_frame;                     //֡��
extern volatile u8 ov_frame_cnt;

void DCMI_init(void);
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc);
void DCMI_Start(void);
void DCMI_Stop(u8 isr);
  
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);

#endif
