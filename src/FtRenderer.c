/*
 * TextRender.c
 *
 *  Created on: Jan 16, 2025
 *      Author: IKOSTADI
 */

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/keycodes.h>
#include <screen/screen.h>
#include <ft2build.h>
#include FT_FREETYPE_H


#include "FtRenderer.h"
#include "logger.h"


FT_Library  library = NULL;    /* handle to library     */
FT_Face     face;                /* handle to face object */

/* Utility function to calculate the dpi based on the display physical properties */
int ftCalcDpi(int width_mm, int height_mm, int resolution_width, int resolution_height) {

    int diagonal_pixels = sqrt(pow(resolution_width, 2) + pow(resolution_height, 2));
    double diagonal_inches = 0.0393700787 * sqrt(pow(width_mm, 2) + pow(height_mm, 2));
    return (int)(diagonal_pixels / diagonal_inches);
}

static void draw_bitmap(FT_GlyphSlot slot, fr_textBox BBox, fr_penPos penPos, fr_textBox *textDirtyRect, fr_grBufferProps buffData ) {
  FT_Bitmap*  bitmap = &slot->bitmap;
  FT_Int      dest_x, dest_y; //Loop vars
  FT_Int      x_max, y_max; //Loop vars
  FT_Int	  x, y;
  unsigned char * srcBuffer = bitmap->buffer;
  int rgbaVal = 0xff;

 // _uint8 *fr_pix_buf_data = buffData.fr_pix_buf_data

  dest_x = BBox.bb_start_x + penPos.pen_x / 64 + slot->bitmap_left;
  dest_y = BBox.bb_start_y + penPos.pen_y / 64 - slot->bitmap_top;


  if (dest_x + bitmap->width <= BBox.bb_start_x + BBox.bb_width) {
    x_max =  bitmap->width;
  } else {
    x_max = BBox.bb_start_x + BBox.bb_width - dest_x;
  }

  if (dest_y + bitmap->rows <= BBox.bb_start_y + BBox.bb_height) {
    y_max =  bitmap->rows;
  } else {
    y_max = BBox.bb_start_y + BBox.bb_height - dest_y;
  }


  //log_message(LOG_DEBUG, "draw_bitmap before loop: x:%d, y:%d, x_max:%d, y_max:%d  ", x, y, x_max, y_max);


  for (y=0; y < y_max; y++) {
	if (dest_y >= BBox.bb_start_y) {
      for (x=0; x < x_max; x++) {
        rgbaVal = 0xff << 24 | srcBuffer[y*(bitmap->width) + x] | srcBuffer[y*(bitmap->width) + x] << 8 | srcBuffer[y*(bitmap->width) + x] << 16;
        //*(int *)&fr_pix_buf_data[((dest_y + y) * fr_buf_size_x * fr_bpp) + ((dest_x + x) * fr_bpp)] = rgbaVal;
        *(int *)&(buffData.fr_pix_buf_data)[((dest_y + y) * buffData.fr_buf_size_x * buffData.fr_bpp) + ((dest_x + x) * buffData.fr_bpp)] = rgbaVal;
      }
    }
  }
}


int ftInitFont(char *fontFile, int point_size, int dpi) {
	FT_Error error = -1;

	error = FT_Init_FreeType( &library );
    if ( error ) {
 	   log_message(LOG_ERROR, "FT_Init_FreeType() returned %d ", error);
 	   return fr_Err_FtInit;
    }

    error = FT_New_Face( library,
                         fontFile,
                         0,
                         &face );
    if ( error )
    {
  	   log_message(LOG_ERROR, "FT_New_Face() returned %d ", error);
 	   return fr_Err_FtFace;
    }

    error = FT_Set_Char_Size (
            face,     			/* handle to face object           */
            point_size * 64, 	/* char_width in 1/64th of points. A point is a physical distance, equaling 1/72th of an inch; it is not a pixel.  */
            point_size * 64, 	/* char_height in 1/64th of points. A 0 value for char_width (char_height ) means same as char_height or (char_width) */
            dpi, 				/* horizontal device resolution. In Dots Per Inch (DPI)  */
            dpi); 				/* vertical device resolution. A 0 value for horizontal or vertical resolution means 72 dpi (default).*/
    if ( error )
    {
   	   log_message(LOG_ERROR, "FT_Set_Char_Size() returned %d ", error);
       return fr_Err_FtSetCharSize;
    }

    return fr_OK;
}

//@fix: This is a Debug function to help text render calculations. Either delete or add handling of different image data modes
void frDrawBoundBox(fr_textBox textBBox, int rgbaVal, fr_grBufferProps buffData) {
	int x;
	int y;

    for (y = textBBox.bb_start_y; y < textBBox.bb_start_y + textBBox.bb_height; y++) {
    	//fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + (textBBox.bb_start_x * fr_bpp) + 0] = (rgbaVal >> 24) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + (textBBox.bb_start_x * fr_bpp) + 1] = (rgbaVal >> 16) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + (textBBox.bb_start_x * fr_bpp) + 2] = (rgbaVal >> 8) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + (textBBox.bb_start_x * fr_bpp) + 3] = (rgbaVal >> 0) & 0xFF;
        *(int*)&(buffData.fr_pix_buf_data)[(y * buffData.fr_buf_size_x * buffData.fr_bpp) + (textBBox.bb_start_x * buffData.fr_bpp)] = rgbaVal;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + ((textBBox.bb_start_x + textBBox.bb_width) * fr_bpp) + 0] = (rgbaVal >> 24) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + ((textBBox.bb_start_x + textBBox.bb_width) * fr_bpp) + 1] = (rgbaVal >> 16) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + ((textBBox.bb_start_x + textBBox.bb_width) * fr_bpp) + 2] = (rgbaVal >> 8) & 0xFF;
        //fr_pix_buf_data[(y * fr_buf_size_x * fr_bpp) + ((textBBox.bb_start_x + textBBox.bb_width) * fr_bpp) + 3] = (rgbaVal >> 0) & 0xFF;
        *(int*)&(buffData.fr_pix_buf_data)[(y * buffData.fr_buf_size_x * buffData.fr_bpp) + ((textBBox.bb_start_x + textBBox.bb_width) * buffData.fr_bpp)] = rgbaVal;
    }
    for (x = textBBox.bb_start_x; x < textBBox.bb_start_x + textBBox.bb_width; x++) {
    	//fr_pix_buf_data[(textBBox.bb_start_y * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 0] = (rgbaVal >> 24) & 0xFF;
    	//fr_pix_buf_data[(textBBox.bb_start_y * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 1] = (rgbaVal >> 16) & 0xFF;
    	//fr_pix_buf_data[(textBBox.bb_start_y * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 2] = (rgbaVal >> 8) & 0xFF;
    	//fr_pix_buf_data[(textBBox.bb_start_y * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 3] = (rgbaVal >> 0) & 0xFF;
    	*(int*)&(buffData.fr_pix_buf_data)[(textBBox.bb_start_y * buffData.fr_buf_size_x * buffData.fr_bpp) + (x * buffData.fr_bpp)] = rgbaVal;
    	//fr_pix_buf_data[((textBBox.bb_start_y + textBBox.bb_height) * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 0] = (rgbaVal >> 24) & 0xFF;
    	//fr_pix_buf_data[((textBBox.bb_start_y + textBBox.bb_height) * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 1] = (rgbaVal >> 16) & 0xFF;
    	//fr_pix_buf_data[((textBBox.bb_start_y + textBBox.bb_height) * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 2] = (rgbaVal >> 8) & 0xFF;
    	//fr_pix_buf_data[((textBBox.bb_start_y + textBBox.bb_height) * fr_buf_size_x * fr_bpp) + (x * fr_bpp) + 3] = (rgbaVal >> 0) & 0xFF;
    	*(int*)&(buffData.fr_pix_buf_data)[((textBBox.bb_start_y + textBBox.bb_height) * buffData.fr_buf_size_x * buffData.fr_bpp) + (x * buffData.fr_bpp)] = rgbaVal;
    }

}

//int ftRender(fr_textBox textBoundBox, fr_penPos penPos, fr_textBox *textDirtyRect, const char* text) {
int ftRender(fr_grBufferProps buffData, fr_canvasProps *pftCanvasProps, const char* text) {
    FT_GlyphSlot  slot;
    FT_Error      error;
    int           n;
    size_t        strLen = strlen(text);

    if (face == NULL) {
        log_message(LOG_ERROR, "ftRender() called with NULL face. Is FT Uninit?");
    	return fr_Err_Generic;
    } else {
        slot = face->glyph;

    	//frDrawBoundBox(textBoundBox, 0x7F7F7F7F);

        for ( n = 0; n < strLen; n++ )
        {
          /* load glyph image into the slot (erase previous one) */
          error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
          if ( error ) continue;  /* ignore errors */

          /* now, draw to our target surface */
          draw_bitmap(slot, pftCanvasProps->txtBoundBox, pftCanvasProps->penPos, &(pftCanvasProps->txtDirtyRect), buffData);

          /* increment pen position */
          pftCanvasProps->penPos.pen_x += slot->advance.x;
          pftCanvasProps->penPos.pen_y += slot->advance.y;

        }
        return fr_OK;
    }
}


void frCalcStrPixelSize(int* penPos_y, int* strWidth, int* strHeight, const char* textStr) {
    *strWidth = 0;
    *strHeight = 0;
//    int max_height = 0;
//    int max_pen_y = 0;
    int pen_x = 0;
    int pen_y = 0;
    int maxBitmapBearringY = 0;
    int minBitmapBearringY = 0;
    int maxBitmapHeight = 0;

    for (const char* p = textStr; *p; p++) {
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER)) {
            continue; // Skip glyph if it cannot be loaded
        }

        FT_GlyphSlot glyph = face->glyph;

        // Update width and height
        pen_x += glyph->advance.x >> 6;
        pen_y += glyph->advance.y >> 6;

        //Find the highest point in all glyphs
        if (glyph->bitmap_top > maxBitmapBearringY) {
            maxBitmapBearringY = glyph->bitmap_top;
        }
        // Find the lowest point in all glyphs. looking for a negative number, so init to 0 is OK.
        if ((int)glyph->bitmap_top - (int)glyph->bitmap.rows < (int)minBitmapBearringY) {
            minBitmapBearringY = glyph->bitmap_top - glyph->bitmap.rows;
        }

        //Store the largest bitmap height
        if (glyph->bitmap.rows > maxBitmapHeight) {
            maxBitmapHeight = glyph->bitmap.rows;
        }
    }

    *penPos_y = maxBitmapBearringY;
    *strWidth = pen_x;
    *strHeight = maxBitmapBearringY + (-minBitmapBearringY);
    if (*strHeight < maxBitmapHeight) {
        log_message(LOG_ERROR, "frCalcStrPixelSize() calculated strHeight=%d less than maxBitmapHeight=%d", strHeight, maxBitmapHeight);

    }
}

