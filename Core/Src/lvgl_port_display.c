/*********************
 *      INCLUDES
 *********************/

#include "lvgl_port_display.h"
#include "main.h"
#include "ltdc.h"
#include "dma2d.h"

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void disp_flush (lv_display_t *, const lv_area_t *, uint8_t *);
static void disp_flush_complete (DMA2D_HandleTypeDef*);
static void disp_flush_error (DMA2D_HandleTypeDef *hdma2d);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t * disp;
static __attribute__((aligned(32))) uint8_t buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES * 2];
//static __attribute__((aligned(32))) uint8_t buf_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];
//static __attribute__((aligned(32))) uint8_t buf_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_display_init (void)
{
	/* display initialization */

	disp = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
	lv_display_set_buffers(disp, buf_1, NULL, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
	lv_display_set_flush_cb(disp, disp_flush);

	/* interrupt callback for DMA2D transfer */
	hdma2d.XferCpltCallback = disp_flush_complete;
	hdma2d.XferErrorCallback = disp_flush_error;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void
disp_flush (lv_display_t * display,
            const lv_area_t * area,
            uint8_t * px_map)
{

  lv_coord_t width = lv_area_get_width(area);
  lv_coord_t height = lv_area_get_height(area);

 uint16_t * fb = hltdc.LayerCfg[0].FBStartAdress;

//  fb += MY_DISP_HOR_RES * area->y1 + area->x1;
//    int x ,y;
//    for(y = 0; y < height; y++) {
//   	  for(x = 0; x < width; x++) {
//   		  fb[x] = ((px_map[2] & 0xF8) << 8) +
//                    ((px_map[1] & 0xFC) << 3) +
//                    ((px_map[0] & 0xF8) >> 3);
//   		  px_map+=4;
//   	  }
//   	  fb+=MY_DISP_HOR_RES;
  
//    }
//  int x ,y;ttt
//  for(y = 0; y < height; y++) {
//	  lv_memcpy(fb, px_map, 2 * width);
//	  fb+=MY_DISP_HOR_RES;
//	  px_map += width * 2;
//  }
  //  lv_memcpy(fb, px_map, 2 * lv_area_get_size(area));


//  lv_display_flush_ready(disp);
//  return;

//  extern DCACHE_HandleTypeDef hdcache1;
//  HAL_DCACHE_Invalidate(&hdcache1);

  DMA2D->CR = 0x1U << DMA2D_CR_MODE_Pos;
#if LV_COLOR_DEPTH == 16
  DMA2D->FGPFCCR = DMA2D_INPUT_RGB565;
#elif LV_COLOR_DEPTH == 32
  DMA2D->FGPFCCR = DMA2D_INPUT_ARGB8888;
#else
#warning dma2d flushing with LV_COLOR_DEPTH other than 16 or 32 is not supported
#endif
  DMA2D->FGMAR = (uint32_t)px_map;
  DMA2D->FGOR = 0;
  DMA2D->OPFCCR = DMA2D_OUTPUT_RGB565;
  DMA2D->OMAR = hltdc.LayerCfg[0].FBStartAdress + 2 * \
                (area->y1 * MY_DISP_HOR_RES + area->x1);
  DMA2D->OOR = MY_DISP_HOR_RES - width;
  DMA2D->NLR = (width << DMA2D_NLR_PL_Pos) | (height << DMA2D_NLR_NL_Pos);
  DMA2D->IFCR = 0x3FU;
  DMA2D->CR |= DMA2D_CR_TCIE;
  DMA2D->CR |= DMA2D_CR_START;
}

static void
disp_flush_complete (DMA2D_HandleTypeDef *hdma2d)
{
  lv_display_flush_ready(disp);
}


static void
disp_flush_error (DMA2D_HandleTypeDef *hdma2d)
{
  while(1);
}

