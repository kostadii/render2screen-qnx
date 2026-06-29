/*
 * ImgLib.h
 *
 *  Created on: Dec 19, 2024
 *      Author: IKOSTADI
 */

#ifndef SRC_LIB_IMGLIB_IMGLIB_IMGLIB_H_
#define SRC_LIB_IMGLIB_IMGLIB_IMGLIB_H_

typedef enum {
  eTxtSrc_NONE = 0,
  eTxtSrc_PARAM,
  eTxtSrc_ENVVAR,
  eTxtSrc_PIPE
} eTextSources;

typedef enum {
  eImgSrc_NONE = 0,
  eImgSrc_PARAM,
  eImgSrc_PIPE
} eImageSources;

typedef enum {
  ePipe_Error_Unknown = -100,
  ePipe_Error_Open = -99,
  ePipe_Error_Read = -98,
  ePipe_Success = 0,
  ePipe_Warning_Empty = 1,
  ePipe_Warning_Overflow = 2,
  ePipe_Warning_Missing = 3
} ePipeResult;


typedef enum {
  eHandleUninit = 0,
  eHandleValid,
  eHandleUbound
} egfxHandleState;


typedef struct {
  screen_context_t scrCtx;
  egfxHandleState scrCtxState;
  int scrFlags;
  screen_window_t scrWin;
  egfxHandleState scrWinState;
  screen_buffer_t scrWinBuffer;
  int scrWinSize[2];
  int scrWinBufferSize[2];
  int scrWinDirtyRect[4];
  int scrWinFormat;
  int scrWinUsage;
  int scrWinRotation;
  int scrWinScaleMode;
  int scrWinMirrorMode;
  void *pScrWin_UserData;
  screen_display_t scrDisp;
  int scrDispDpi;
  int scrClearNeeded;
  int scrRedrawNeeded;
} bgrScrWinContexts;

typedef struct {
  char onScr_imgFileName[PARAM_MAX_LENGTH];
  char onScr_ttfFileName[PARAM_MAX_LENGTH];
  char onScr_textString[PARAM_MAX_LENGTH];
} bgrOnScreenContentsData;

typedef struct {
  screen_pixmap_t imgPixmap;
  egfxHandleState imgPixmapState;
  screen_buffer_t imgPixmapBuffer;
  egfxHandleState imgPixmapBufferState;
  img_t img;
  char imgFileName[PARAM_MAX_LENGTH];
  img_fixed_t imgRotationAngle;
  int imgRedrawNeeded;
} bgrImgPixmapData;


typedef struct {
  screen_pixmap_t txtPixmap;
  egfxHandleState txtPixmapState;
  screen_buffer_t txtPixmapBuffer;
  egfxHandleState txtPixmapBufferState;
  int txtPixmapSize;
  void *pTxtPixmapBuffer;
  int txtPixmapBufferStride;
  char ttfFileName[PARAM_MAX_LENGTH];
  unsigned char fontSize;
  fr_canvasProps ftCanvasProps;
  int txtRedrawNeeded;
} bgrTxtPixmapData;


void rotateImage180(unsigned char *data, int width, int height, int bytesPerPixel);
int getBytesPerPixel(img_t img);
//static int decode_setup(uintptr_t data, img_t *img, unsigned flags);
//static void decode_abort(uintptr_t data, img_t *img);
int load_image(screen_window_t screen_win, const char *path);
int LoadImg(img_t *img, const char *path);

//int createWindow(screen_context_t *pScreen_ctx, screen_window_t *pScreen_win, int *screen_size, int *buffer_size);
int bgrCreateWindow(bgrScrWinContexts *pScrWinCtxt);
int createWindowBuffers(screen_window_t *pScreen_win, void **screen_buf);
int displayWindowBuffer(screen_window_t *pScreen_win, screen_buffer_t screen_bufer, int *dirty_rect);
int bgrCreatePixmap(screen_context_t *pScreen_ctx, screen_pixmap_t *pScreen_pix);
int bgrResetTxtPixmapBuffer(bgrTxtPixmapData *pTxtPixmapData, int *pixmap_size, fr_grBufferProps *pBuffProps);
int bgrLoadImagePixmap(bgrImgPixmapData *pImgPxmpData);
int bgrBlitImagePixmap(bgrImgPixmapData *imgPxmpData, bgrScrWinContexts *pScrWinCtxt);
void bgrCleanupScrWinContexts (bgrScrWinContexts *pScrWinCtxt);
void bgrCleanupImgPxmpContexts (bgrImgPixmapData *imgPxmpData);
void bgrCleanupTxtPxmpContexts (bgrTxtPixmapData *txtPxmpData);
int bgrGetScreenDpi(bgrScrWinContexts *pScrWinCtxt);
ePipeResult bgrGetFifoText(char *txtStr, int maxTxtSize, const char* appPipeName);

#endif /* SRC_LIB_IMGLIB_IMGLIB_IMGLIB_H_ */
