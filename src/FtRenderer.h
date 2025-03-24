/*
 * TextRender.h
 *
 *  Created on: Jan 16, 2025
 *      Author: IKOSTADI
 */

#ifndef SRC_LIB_IMGLIB_IMGLIB_FTRENDER_H_
#define SRC_LIB_IMGLIB_IMGLIB_FTRENDER_H_

typedef struct {
  int bb_start_x;
  int bb_start_y;
  int bb_width;
  int bb_height;
} fr_textBox;

typedef struct {
	int pen_x;
	int pen_y;
} fr_penPos;

typedef struct {
  fr_textBox txtBoundBox;
  fr_textBox txtDirtyRect;
  fr_penPos penPos;
} fr_canvasProps;

typedef struct {
  _uint8 *fr_pix_buf_data;
  int fr_buf_size_x;
  int fr_buf_size_y;
  int fr_bpp;
} fr_grBufferProps;

typedef enum {
	fr_OK,
	fr_Err_Generic,
	fr_Err_FtInit,
	fr_Err_FtFace,
	fr_Err_FtSetCharSize
} frErrorType;

int ftCalcDpi(int width_mm, int height_mm, int resolution_width, int resolution_height);
int ftInitDestBuffer (_uint8 *pix_buf_data, int buf_size_x, int buf_size_y, int bpp);
int ftInitFont(char *fontFile, int point_size, int dpi);
int ftRender(fr_grBufferProps buffData, fr_canvasProps *pftCanvasProps, const char* text);
void frCalcStrPixelSize(int* penPos_y, int* strWidth, int* strHeight, const char* textStr);


#endif /* SRC_LIB_IMGLIB_IMGLIB_FTRENDER_H_ */
