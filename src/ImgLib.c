/**
 ******************************************************************************
 *
 *  @file ImgLib.c
 *
 *  @brief Image Display Utility.
 *
 *
 ******************************************************************************
*/

/******************************************************************************
  Depends
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __QNX__
 #include <time.h>
 #include <screen/screen.h>
 #include <sys/netmgr.h>
 #include <sys/neutrino.h>
 #include <dirent.h>
 #include <img/img.h>
 #include <errno.h>
 #include <fcntl.h>
 #include <unistd.h>
#endif

#include "logger.h"
#include "argParse.h"
#include "FtRenderer.h"
#include "ImgLib.h"


/******************************************************************************
  Macro/Constant Definitions
 ******************************************************************************/
#define TFT_WIDTH_MM 174
#define TFT_HEIGHT_MM 104
#define TFT_HORIZONTAL_RESOLUTION 1280
#define TFT_VERTICAL_RESOLUTION 768
#define FONT_SIZE_MAX 240
#define FONT_SIZE_MIN 4

/******************************************************************************
  Type Definitions
 ******************************************************************************/

/******************************************************************************
  File Scope Variables
 ******************************************************************************/

/******************************************************************************
  File Scope Function Prototypes
 ******************************************************************************/

/******************************************************************************
  File Scope Functions
 ******************************************************************************/

/******************************************************************************
  Global Functions
 ******************************************************************************/



/******************************************************************************
  Arguments validation functions
 ******************************************************************************/


// Look for valid file
int validate_img(const char *value) {
    int result = 0;

    if (value == NULL || strlen(value) == 0) {
        //printf("Empty path is invalid\n");
        log_message(LOG_WARNING, "validate_img(): Empty path is invalid");
        result = 0;
    } else {
        log_message(LOG_DEBUG, "validate_img(): File parameter passed: %s", value);
        // Check for file extension
        const char *ext = strrchr(value, '.');
        if (ext == NULL) {
            //printf("No file extension found\n");
            log_message(LOG_WARNING, "validate_img(): No file extension found");
            result = 0;
        } else {
            // Compare extension with allowed image formats (case-insensitive)
            log_message(LOG_DEBUG, "validate_img(): File extension: %s", ext);
            if ( (strcasecmp(ext, ".png") == 0) || (strcasecmp(ext, ".jpg") == 0) || (strcasecmp(ext, ".jpeg") == 0) || (strcasecmp(ext, ".bmp") == 0) ) {
                // Basic file existence check (may not be reliable on all systems)
                FILE* file = fopen(value, "rb");
                if (file == NULL) {
                    //printf("File could not be opened\n");
                    log_message(LOG_WARNING, "validate_img(): File could not be opened");
                    result = 0;
                } else {
                    fclose(file);
                    //printf("File exists\n");
                    log_message(LOG_INFO, "validate_img() passed");
                    result = 1;
                }
            } else {
                //printf("Extension is not of a recognized image format\n");
                log_message(LOG_WARNING, "validate_img(): Extension is not of a recognized image format");
                result = 0;
            }
        }
    }

    return result;
}

int validate_img_source(const char *value) {
    int result = 0;

    if (value) {
        if ( strcmp(value, "NONE") == 0 ) {
            result = 1;
        } else if ( strcmp(value, "PARAM") == 0 ) {
                result = 1;
        } else if ( strcmp(value, "NPFIFO") == 0 ) {
            result = 1;
        } else {
            result = 0;
        }
    }

    return result;
}

int validate_img_fifo(const char *value) {
    int result = 0;
    int fd;

    if (value == NULL || strlen(value) == 0) {
        //printf("Empty path is invalid\n");
        log_message(LOG_WARNING, "validate_img_fifo(): Empty path is invalid");
        result = 0;
    } else {
        log_message(LOG_DEBUG, "validate_img_fifo(): Image FIFO parameter passed: %s", value);
        fd = open(value, O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            log_message(LOG_ERROR, "validate_img_fifo(): Image FIFO File could not be opened");
            result = 0;
        } else {
            close(fd);
            log_message(LOG_INFO, "validate_img_fifo() passed");
            result = 1;
        }
    }

    return result;
}


int validate_rotation(const char *value) {
    int result = 0;
    if (value) {
        if ( strcmp(value, "0") == 0 ) {
            result = 1;
        } else if ( strcmp(value, "90") == 0) {
            result = 1;
        } else if ( strcmp(value, "180") == 0) {
            result = 1;
        } else if ( strcmp(value, "270") == 0) {
            result = 1;
        } else {
            result = 0;
        }
    }

    return result;
}


int validate_scale(const char *value) {
    int result = 1;

    if (value) {
        if ( strcmp(value, "NONE") == 0 ) {
        } else if ( strcmp(value, "STRETCH") == 0) {
        } else if ( strcmp(value, "ZOOM") == 0) {
        } else if ( strcmp(value, "FILL") == 0) {
        } else if ( strcmp(value, "SHIFT_UP") == 0) {
        } else if ( strcmp(value, "SHIFT_DOWN") == 0) {
        } else {
            result = 0;
        }
    }

    return result;
}


int validate_mirror(const char *value) {
    int result = 1;

    if (value) {
        if ( strcmp(value, "DISABLED") == 0 ) {
        } else if ( strcmp(value, "NORMAL") == 0) {
        } else if ( strcmp(value, "STRETCH") == 0) {
        } else if ( strcmp(value, "ZOOM") == 0) {
        } else if ( strcmp(value, "FILL") == 0) {
        } else {
            result = 0;
        }
	}

    return result;
}


int validate_verbosity(const char *value) {
    int result = 1;

    if (value) {
        if (strcmp(value, "1") == 0) {
            log_init(LOG_ERROR);
        } else if (strcmp(value, "2") == 0) {
            log_init(LOG_WARNING);
        } else if (strcmp(value, "3") == 0) {
            log_init(LOG_INFO);
        } else if (strcmp(value, "4") == 0) {
            log_init(LOG_DEBUG);
        } else {
            log_init(LOG_DEFAULT);
            result = 0;
        }
	}

    return result;
}

int validate_font_file(const char *value) {

    int result = 0;

    if (value == NULL || strlen(value) == 0) {
        log_message(LOG_WARNING, "validate_fontFile(): Empty font path_name is invalid");
        result = 0;
    } else {
        log_message(LOG_DEBUG, "validate_fontFile(): Font parameter passed: %s", value);
        const char *ext = strrchr(value, '.');
        if (ext == NULL) {
            log_message(LOG_WARNING, "validate_fontFile(): No font file extension found");
            result = 0;
        } else {
            log_message(LOG_DEBUG, "validate_fontFile(): Font file extension: %s", ext);
            if ( (strcasecmp(ext, ".ttf") == 0) || (strcasecmp(ext, ".otf") == 0) ) {
                // Basic file existence check (may not be reliable on all systems)
                FILE* file = fopen(value, "rb");
                if (file == NULL) {
                    log_message(LOG_WARNING, "validate_fontFile(): Font file could not be opened");
                    result = 0;
                } else {
                    fclose(file);
                    log_message(LOG_INFO, "validate_fontFile(): Passed");
                    result = 1;
                }
            } else {
                log_message(LOG_WARNING, "validate_fontFile(): Font Extension is not of a supported format");
                result = 0;
            }
        }
    }

	return result;
}

int validate_font_size(const char *value) {
  int strLen = 1;
  int num = 0;
  const char *p = value;

  if (value == NULL || *value == '\0') {
    return 0;
  } else {
    //Is a numeric value passed?
    for (; *p && strLen <=2; ++p) {
      if ((*p < 0x30)||(*p>0x39)) {
        return 0;
      } else {
        strLen++;
      }
    }
    // Does the value check out?
    num = atoi(value);
    if ((num >= FONT_SIZE_MIN) && (num <= FONT_SIZE_MAX) && (FONT_SIZE_MAX <= 255)) {
      return 1;
    } else {
      return 0;
    }
  }
}


int validate_text(const char *value) {
    int result = 0;
    if (value == NULL || strlen(value) == 0) {
        log_message(LOG_WARNING, "validate_text(): The passed text is NULL or zero sized");
        result = 0;
    } else {
        log_message(LOG_INFO, "validate_text(): Passed");
        result = 1;
    }
    return result;
}

int validate_text_source(const char *value) {
	int result = 1;

    if (value) {
        if ( strcmp(value, "NONE") == 0 ) {
        } else if ( strcmp(value, "PARAM") == 0 ) {
            result = 1;
        } else if ( strcmp(value, "ENVVAR") == 0 ) {
            result = 1;
        } else if ( strcmp(value, "NPFIFO") == 0 ) {
            result = 1;
        } else {
            result = 0;
        }
    }

	return result;
}

int validate_text_fifo(const char *value) {
    int result = 0;
    int fd;

    if (value == NULL || strlen(value) == 0) {
        //printf("Empty path is invalid\n");
        log_message(LOG_WARNING, "validate_text_fifo(): Empty path is invalid");
        result = 0;
    } else {
        log_message(LOG_DEBUG, "validate_text_fifo(): Image FIFO parameter passed: %s", value);
        fd = open(value, O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            log_message(LOG_ERROR, "validate_text_fifo(): Image FIFO File could not be opened");
            result = 0;
        } else {
            close(fd);
            log_message(LOG_INFO, "validate_text_fifo() passed");
            result = 1;
        }
    }

    return result;
}

int validate_text_hpos(const char *value) {
  int result = 1;

  if (value) {
    if ( strcmp(value, "LEFT") == 0 ) {
      result = 1;
    } else if ( strcmp(value, "RIGHT") == 0 ) {
      result = 1;
    } else if ( strcmp(value, "CENTER") == 0 ) {
      result = 1;
    } else {
      result = 0;
    }
  }

  return result;
}

int validate_text_vpos(const char *value) {
  int result = 1;

  if (value) {
    if ( strcmp(value, "TOP") == 0 ) {
      result = 1;
    } else if ( strcmp(value, "BOTTOM") == 0 ) {
      result = 1;
    } else if ( strcmp(value, "CENTER") == 0 ) {
      result = 1;
    } else {
      result = 0;
    }
  }

  return result;
}



// Enumeration for parameter indices
typedef enum {
    PARAM_VERBOCITY,
    PARAM_IMG_FILE,
    PARAM_IMG_SOURCE,
    PARAM_IMG_FIFO,
    PARAM_ROTATION,
    PARAM_SCALE,
    PARAM_MIRROR,
    PARAM_FONT_FILE,
    PARAM_FONT_SIZE,
    PARAM_TEXT,
    PARAM_TEXT_SOURCE,
    PARAM_TEXT_FIFO,
    PARAM_TEXT_HPOS,
    PARAM_TEXT_VPOS,
    PARAM_COUNT // Automatically provides the count of parameters
} ParameterIndex;

// Define the array of parameters
char imgFifoName[PARAM_MAX_LENGTH] = "/tmp/imgLib-arm_imgFifo";
char txtFifoName[PARAM_MAX_LENGTH] = "/tmp/imgLib-arm_txtFifo";

tCmdOptionParam params[] = {
    {"-v", 				"", 	validate_verbosity, 	"[-v=1..4]", 													"Verbosity (optional): 1-Error, 2-Warning+, 3-Info+, 4-Debug+.", 									false, 	false, 	"1"							},
    {"-imgFile", 		"", 	validate_img, 			"[-imgFile=fullPathToFile]", 									"Path to the input file (required).", 																false, 	false, 	NULL						},
    {"-imgSrc", 		"", 	validate_img_source, 	"[-imgSrc={NONE|PARAM|NPFIFO}]", 								"Source of image file (Optional). PARAM: -imgFile, NPFIFO: -imgFifo. Default: PARAM", 				false, 	false, 	"PARAM"						},
    {"-imgFifo", 		"", 	validate_img_fifo, 		"[-imgFifo=fullPathToImgFifoFile", 								"The path/name of the text FIFO file (optional). Default: <CrntDir/AppName>_imgFifo", 				false, 	false, 	imgFifoName					},
    {"-scrRotation",	"", 	validate_rotation, 		"[-scrRotation={0|90|180|270}]", 								"Rotation angle (optional): Clockwise, multiple of 90. Default: 0.", 								false, 	false, 	"0"							},
    {"-scrScaleMd", 	"", 	validate_scale, 		"[-scrScaleMd={NONE|STRETCH|ZOOM|FILL|SHIFT_UP|SHIFT_DOWN}]", 	"Scale factor (optional): None or one of the listed types.", 										false, 	false, 	"NONE"						},
    {"-scrMirrMd",		"", 	validate_mirror, 		"[-scrMirrMd={DISABLED|NORMAL|STRETCH|ZOOM|FILL}]", 			"Mirror Mode (optional): Disabled or one of the listed modes.", 									false, 	false, 	"DISABLED"					},
    {"-fontFile",		"", 	validate_font_file, 	"[-fontFile=fullPathToFontFile]",								"Font file to use (optional). Default: /usr/fonts/DejaVuSans.ttf", 									false, 	false, 	"/usr/fonts/DejaVuSans.ttf"	},
    {"-fontSize",		"", 	validate_font_size, 	"[-fontSize=FintSizeInPoints]",									"Font size to use (optional). Range 4..32. Default: 16 points", 									false, 	false, 	"16"						},
    {"-txt",			"", 	validate_text, 			"[-txt=\"Display text\"]",										"Quote enclosed non-null text to display (optional). Default: Error text.",							false, 	false, 	"No -txt= passed" 			},
    {"-txtSrc",			"", 	validate_text_source, 	"[-txtSrc={NONE|PARAM|ENVVAR|NPFIFO}]",							"ENVVAR: BOOT_TEXT_STR=\"..\"; PARAM: -text=\"..\"; NPFIFO: -textFifo; Default: PARAM",				false, 	false, 	"PARAM"			 			},
    {"-txtFifo",		"", 	validate_text_fifo, 	"[-txtFifo=fullPathToTextFifoFile]",							"The path/name of the text FIFO file (optional). Default: <CrntDir/AppName>_txtFifo",				false, 	false, 	txtFifoName					},
    {"-txtHPos",		"", 	validate_text_hpos, 	"[-txtHPos=LEFT|CENTER|RIGHT]",									"Text horizontal alignment (optional). Default: LEFT",												false, 	false, 	"LEFT"						},
    {"-txtVPos",		"", 	validate_text_vpos, 	"[-txtVPos=TOP|CENTER|BOTOM]",									"Text vertical alignment (optional). Default: BOTTOM",												false, 	false, 	"BOTTOM"					}
};


int getBytesPerPixel(img_t img) {
    int retval;

    // @fix: Using switch for now. In future just use flags or mask and shift:
    // IMG_FMT_PKLE_ARGB8888 = 32 | IMG_FMT_PKLE | IMG_FMT_ALPHA | IMG_FMT_RGB,
    switch (img.format) {
        case (IMG_FMT_PKLE_ABGR8888):
        case (IMG_FMT_PKBE_ABGR8888):
        case (IMG_FMT_PKLE_XBGR8888):
        case (IMG_FMT_PKBE_XBGR8888):
        case (IMG_FMT_PKLE_ARGB8888):
        case (IMG_FMT_PKBE_ARGB8888):
        case (IMG_FMT_PKLE_XRGB8888):
        case (IMG_FMT_PKBE_XRGB8888):
            retval = 4;
            break;
        case (IMG_FMT_BGR888):
        case (IMG_FMT_RGB888):
        case (IMG_FMT_YUV888):
            retval = 3;
            break;
        case (IMG_FMT_PKLE_RGB565):
        case (IMG_FMT_PKBE_RGB565):
        case (IMG_FMT_PKLE_ARGB1555):
        case (IMG_FMT_PKBE_ARGB1555):
        case (IMG_FMT_PKLE_XRGB1555):
        case (IMG_FMT_PKBE_XRGB1555):
            retval = 2;
            break;
        case (IMG_FMT_G8):
        case (IMG_FMT_A8):
        case (IMG_FMT_PAL8):
            retval = 1;
            break;
        case (IMG_FMT_MONO):
        case (IMG_FMT_PAL1):
        case (IMG_FMT_PAL4):
            retval = 1; //Round up to 1 byte
            break;
        case (IMG_FMT_INVALID):
            retval = 0;
            break;
        default:
            retval = 0;
    }

    return retval;
}

// Don't use!
void rotateImage180(unsigned char *data, int width, int height, int bytesPerPixel) {
    int halfHeight = height / 2;

    for (int row = 0; row < halfHeight; ++row) {
        for (int col = 0; col < width; ++col) {
            for (int byte = 0; byte < bytesPerPixel; ++byte) {
                int topIndex = (row * width + col) * bytesPerPixel + byte;
                int bottomIndex = ((height - 1 - row) * width + (width - 1 - col)) * bytesPerPixel + byte;

                // Swap the pixels
                unsigned char temp = data[topIndex];
                data[topIndex] = data[bottomIndex];
                data[bottomIndex] = temp;
            }
        }
    }

    // If the height is odd, we need to reverse the middle row
    if (height % 2 != 0) {
        int middleRow = halfHeight;
        for (int col = 0; col < width / 2; ++col) {
            for (int byte = 0; byte < bytesPerPixel; ++byte) {
                int leftIndex = (middleRow * width + col) * bytesPerPixel + byte;
                int rightIndex = (middleRow * width + (width - 1 - col)) * bytesPerPixel + byte;

                // Swap the pixels
                unsigned char temp = data[leftIndex];
                data[leftIndex] = data[rightIndex];
                data[rightIndex] = temp;
            }
        }
    }
}



static int ilDecodeSetupPixmap(uintptr_t data, img_t *img, unsigned flags)
{
  bgrImgPixmapData *pImgPixmapData = (bgrImgPixmapData *)data;
  int size[2];
  int screenIfaceResult = -1;

  size[0] = img->w;
  size[1] = img->h;
  //screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
  screenIfaceResult = screen_set_pixmap_property_iv(pImgPixmapData->imgPixmap, SCREEN_PROPERTY_BUFFER_SIZE, size);
  if (screenIfaceResult != EOK) {
    log_message(LOG_ERROR, "decode_setup_pixmap::screen_set_pixmap_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) with size[0]:%d, size[0]:%d returned non-zero: %d", size[0], size[1], screenIfaceResult);
    screenIfaceResult = -1;
  } else {
    screenIfaceResult = screen_create_pixmap_buffer(pImgPixmapData->imgPixmap);
    if (screenIfaceResult != EOK) {
      log_message(LOG_ERROR, "decode_setup_pixmap::screen_create_pixmap_buffer()  returned non-zero: %d", screenIfaceResult);
      screenIfaceResult = -2;
    } else {
      screenIfaceResult = screen_get_pixmap_property_pv(pImgPixmapData->imgPixmap, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&(pImgPixmapData->imgPixmapBuffer));
      if (screenIfaceResult != EOK) {
        log_message(LOG_ERROR, "decode_setup_pixmap::screen_get_pixmap_property_pv(SCREEN_PROPERTY_RENDER_BUFFERS) returned non-zero: %d", screenIfaceResult);
        screenIfaceResult = -3;
      } else {
        pImgPixmapData->imgPixmapBufferState = eHandleValid;
        screenIfaceResult = screen_get_buffer_property_pv(pImgPixmapData->imgPixmapBuffer, SCREEN_PROPERTY_POINTER, (void **)&img->access.direct.data);
        if (screenIfaceResult != EOK) {
          log_message(LOG_ERROR, "decode_setup_pixmap::screen_get_buffer_property_pv(SCREEN_PROPERTY_POINTER) returned non-zero: %d", screenIfaceResult);
          screenIfaceResult = -4;
        } else {
          screenIfaceResult = screen_get_buffer_property_iv(pImgPixmapData->imgPixmapBuffer, SCREEN_PROPERTY_STRIDE, (int *)&img->access.direct.stride);
          if (screenIfaceResult != EOK) {
            log_message(LOG_ERROR, "decode_setup_pixmap::screen_get_buffer_property_pv(SCREEN_PROPERTY_STRIDE) returned non-zero: %d", screenIfaceResult);
            screenIfaceResult = -5;
          } else {
            screenIfaceResult = 0;
            log_message(LOG_DEBUG, "decode_setup_pixmap: pix_buf:%d, img->access.direct.data:%d ", pImgPixmapData->imgPixmapBuffer, img->access.direct.data);
          }
        }
      }
    }
  }

   img->flags |= IMG_DIRECT;

   return screenIfaceResult;
}

static void ilDecodeAbortPixmap(uintptr_t data, img_t *img)
{
  bgrImgPixmapData *pImgPixmapData = (bgrImgPixmapData *)data;
//  screen_window_t screen_pixmap = (screen_window_t)data;
  screen_destroy_pixmap_buffer(pImgPixmapData->imgPixmap);
}
// This version targets render loops support.
int bgrLoadImagePixmap(bgrImgPixmapData *pImgPxmpData)
{
  img_decode_callouts_t callouts;
  img_lib_t ilib = NULL;
  int rc;

  rc = img_lib_attach(&ilib);
  if (rc != IMG_ERR_OK) {
    log_message(LOG_ERROR, "Failed to load lib. Error %d ", rc);
    return -1;
  } else {
    memset(&(pImgPxmpData->img), 0, sizeof(pImgPxmpData->img));
    pImgPxmpData->img.flags |= IMG_FORMAT;
    pImgPxmpData->img.format = IMG_FMT_PKLE_XRGB8888;

    if (pImgPxmpData->imgPixmapBufferState == eHandleValid) {
    	screen_destroy_pixmap_buffer(pImgPxmpData->imgPixmap);
    	pImgPxmpData->imgPixmapBufferState = eHandleUninit;
    }

    memset(&callouts, 0, sizeof(callouts));
    callouts.setup_f = ilDecodeSetupPixmap;
    callouts.abort_f = ilDecodeAbortPixmap;
    callouts.data = (uintptr_t)pImgPxmpData;

    rc = img_load_file(ilib, pImgPxmpData->imgFileName, &callouts, &(pImgPxmpData->img));
    if (rc == IMG_ERR_OK) {
      //printf( "imgdata: img.h:%d , img.w:%d, img.flags:%d, img.format:%d \n",img.h, img.w, img.flags, img.format );
      log_message(LOG_DEBUG,
                  "imgdata: img.h:%d, img.w:%d, img.flags:%d, img.format:%d",
                  pImgPxmpData->img.h, pImgPxmpData->img.w, pImgPxmpData->img.flags, pImgPxmpData->img.format
                 );

      //if (pImgPxmpData->imgRotationAngle == IMG_ANGLE_180) {
        //img_rotate_ortho(&img, &img, IMG_ANGLE_180);
        //log_message(LOG_INFO, "Rotation 180");
        //rotateImage180(pImgPxmpData->img.access.direct.data, pImgPxmpData->img.w, pImgPxmpData->img.h, getBytesPerPixel(pImgPxmpData->img));
      //}
    }

    img_lib_detach(ilib);

    return rc == IMG_ERR_OK ? 0 : -1;
  }
}



void setup_blit_attributes(int src_x, int src_y, int src_width, int src_height,
                           int dst_x, int dst_y, int dst_width, int dst_height,
                           int global_alpha, int transparency, int scale_quality,
                           int *attribs) {
    int index = 0;
    attribs[index++] = SCREEN_BLIT_SOURCE_X;
    attribs[index++] = src_x;
    attribs[index++] = SCREEN_BLIT_SOURCE_Y;
    attribs[index++] = src_y;
    attribs[index++] = SCREEN_BLIT_SOURCE_WIDTH;
    attribs[index++] = src_width;
    attribs[index++] = SCREEN_BLIT_SOURCE_HEIGHT;
    attribs[index++] = src_height;
    attribs[index++] = SCREEN_BLIT_DESTINATION_X;
    attribs[index++] = dst_x;
    attribs[index++] = SCREEN_BLIT_DESTINATION_Y;
    attribs[index++] = dst_y;
    attribs[index++] = SCREEN_BLIT_DESTINATION_WIDTH;
    attribs[index++] = dst_width;
    attribs[index++] = SCREEN_BLIT_DESTINATION_HEIGHT;
    attribs[index++] = dst_height;
    attribs[index++] = SCREEN_BLIT_GLOBAL_ALPHA;
    attribs[index++] = global_alpha;
    attribs[index++] = SCREEN_BLIT_TRANSPARENCY;
    attribs[index++] = transparency;
    attribs[index++] = SCREEN_BLIT_SCALE_QUALITY;
    attribs[index++] = scale_quality;
    attribs[index++] = SCREEN_BLIT_END;
}

void setup_fill_attributes(int dst_x, int dst_y, int dst_width, int dst_height,
                           int global_alpha, int color, int *attribs) {
    int index = 0;

    attribs[index++] = SCREEN_BLIT_DESTINATION_X;
    attribs[index++] = dst_x;
    attribs[index++] = SCREEN_BLIT_DESTINATION_Y;
    attribs[index++] = dst_y;
    attribs[index++] = SCREEN_BLIT_DESTINATION_WIDTH;
    attribs[index++] = dst_width;
    attribs[index++] = SCREEN_BLIT_DESTINATION_HEIGHT;
    attribs[index++] = dst_height;
    attribs[index++] = SCREEN_BLIT_GLOBAL_ALPHA;
    attribs[index++] = global_alpha;
    attribs[index++] = SCREEN_BLIT_COLOR;
    attribs[index++] = color;
    attribs[index++] = SCREEN_BLIT_END;
}


int createWindow(screen_context_t *pScreen_ctx, screen_window_t *pScreen_win, int *screen_size, int *buffer_size) {
    const int windowUsage = SCREEN_USAGE_WRITE | SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE ;
	int createWindowResult = -1;
	int screenIfaceResult;
	//screen_context_t screen_ctx = *pScreen_ctx;
	//screen_window_t screen_win = *pScreen_win;

    screenIfaceResult = screen_create_window(pScreen_win, *pScreen_ctx);
    if (screenIfaceResult == EOK) {
      screenIfaceResult = screen_set_window_property_iv(*pScreen_win, SCREEN_PROPERTY_USAGE, &windowUsage);
      if (screenIfaceResult == EOK) {
        //@Fix: format neither received nor stored.
        screenIfaceResult = screen_set_window_property_iv(*pScreen_win, SCREEN_PROPERTY_FORMAT, (int[]){SCREEN_FORMAT_RGBX8888});
        if (screenIfaceResult == EOK) {
          screenIfaceResult = screen_get_window_property_iv(*pScreen_win, SCREEN_PROPERTY_SIZE, screen_size);
          if (screenIfaceResult == EOK) {
            screenIfaceResult = screen_get_window_property_iv(*pScreen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
            if (screenIfaceResult == EOK) {
              createWindowResult = 0; //EOK
            } else {
                log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) returned non-zero: %d ", screenIfaceResult);
                createWindowResult = -5;
            }
          } else {
              log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_SIZE) returned non-zero: %d ", screenIfaceResult);
              createWindowResult = -4;
          }
        } else {
          log_message(LOG_ERROR, "createWindow::screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT) returned non-zero: %d ", screenIfaceResult);
          createWindowResult = -3;
        }
      } else {
        log_message(LOG_ERROR, "createWindow::screen_set_window_property_iv(SCREEN_PROPERTY_USAGE) returned non-zero: %d ", screenIfaceResult);
        createWindowResult = -2;
      }
    } else {
        log_message(LOG_ERROR, "createWindow::screen_create_window() returned non-zero: %d ", screenIfaceResult);
        createWindowResult = -1;
    }

	return createWindowResult;
}

int bgrCreateWindow(bgrScrWinContexts *pScrWinCtxt) {
  int createWindowResult = -1;
  int screenIfaceResult;

    screenIfaceResult = screen_create_window( &(pScrWinCtxt->scrWin), pScrWinCtxt->scrCtx);
    if (screenIfaceResult == EOK) {
      screenIfaceResult = screen_set_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_USAGE, &(pScrWinCtxt->scrWinUsage));
      if (screenIfaceResult == EOK) {
        screenIfaceResult = screen_set_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_FORMAT, &(pScrWinCtxt->scrWinFormat));
        if (screenIfaceResult == EOK) {
          screenIfaceResult = screen_get_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_SIZE, pScrWinCtxt->scrWinSize);
          if (screenIfaceResult == EOK) {
            screenIfaceResult = screen_get_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_BUFFER_SIZE, pScrWinCtxt->scrWinBufferSize);
            if (screenIfaceResult == EOK) {
                createWindowResult = 0;
                if (pScrWinCtxt->scrWinRotation != 0 ) {
                    screenIfaceResult = screen_set_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_ROTATION, &(pScrWinCtxt->scrWinRotation));
                    if (screenIfaceResult == EOK) {
                        createWindowResult = 0; //EOK
                    } else {
                        log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_ROTATION) returned non-zero: %d ", screenIfaceResult);
                        createWindowResult = -6;
                    }
                }
                if (createWindowResult == 0) {
                    screenIfaceResult = screen_set_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_SCALE_MODE, &(pScrWinCtxt->scrWinScaleMode));
                    if (screenIfaceResult == EOK) {
                        createWindowResult = 0; //EOK
                    } else {
                        log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_SCALE_MODE) returned non-zero: %d ", screenIfaceResult);
                        createWindowResult = -6;
                    }
                }
                if ((createWindowResult == 0) && (pScrWinCtxt->scrWinMirrorMode != SCREEN_MIRROR_DISABLED )) {
                    screenIfaceResult = screen_set_window_property_iv(pScrWinCtxt->scrWin, SCREEN_PROPERTY_MIRROR, &(pScrWinCtxt->scrWinMirrorMode));
                    if (screenIfaceResult == EOK) {
                        createWindowResult = 0; //EOK
                    } else {
                        log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_MIRROR) returned non-zero: %d ", screenIfaceResult);
                        createWindowResult = -6;
                    }
                }

            } else {
                log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) returned non-zero: %d ", screenIfaceResult);
                createWindowResult = -5;
            }
          } else {
              log_message(LOG_ERROR, "createWindow::screen_get_window_property_iv(SCREEN_PROPERTY_SIZE) returned non-zero: %d ", screenIfaceResult);
              createWindowResult = -4;
          }
        } else {
          log_message(LOG_ERROR, "createWindow::screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT) returned non-zero: %d ", screenIfaceResult);
          createWindowResult = -3;
        }
      } else {
        log_message(LOG_ERROR, "createWindow::screen_set_window_property_iv(SCREEN_PROPERTY_USAGE) returned non-zero: %d ", screenIfaceResult);
        createWindowResult = -2;
      }
    } else {
        log_message(LOG_ERROR, "createWindow::screen_create_window() returned non-zero: %d ", screenIfaceResult);
        createWindowResult = -1;
    }

	return createWindowResult;
}

int createWindowBuffers(screen_window_t *pScreen_win, void **screen_buf) {
  int createWindowBuffersResult = -1;
  int screenIfaceResult;

  screenIfaceResult = screen_create_window_buffers(*pScreen_win, 1);
  if (screenIfaceResult == EOK) {
    screenIfaceResult = screen_get_window_property_pv(*pScreen_win, SCREEN_PROPERTY_RENDER_BUFFERS, screen_buf);
    if (screenIfaceResult == EOK) {
      createWindowBuffersResult = 0; //EOK
    } else {
      log_message(LOG_ERROR, "createWindowBuffers::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) returned non-zero: %d ", screenIfaceResult);
      createWindowBuffersResult = -2;
    }
  } else {
    log_message(LOG_ERROR, "createWindowBuffers::screen_create_window_buffers(1) returned non-zero: %d ", screenIfaceResult);
    createWindowBuffersResult = -1;
  }

  return createWindowBuffersResult;
}

int displayWindowBuffer(screen_window_t *pScreen_win, screen_buffer_t screen_bufer, int *dirty_rect) {
  int displayWindowBufferResult = -1;
  int screenIfaceResult;

  screenIfaceResult = screen_set_window_property_iv(*pScreen_win, SCREEN_PROPERTY_VISIBLE, (int[]){1});
  if (screenIfaceResult == EOK) {
    log_message(LOG_DEBUG, "screen_set_window_property_iv(SCREEN_PROPERTY_VISIBLE, 1) completed");
    screenIfaceResult = screen_post_window(*pScreen_win, screen_bufer, 0, dirty_rect, 0);
    //screenIfaceResult = screen_post_window(*pScreen_win, screen_bufer, 0, NULL, 0);
    if (screenIfaceResult == EOK) {
      log_message(LOG_DEBUG, "displayWindowBuffer::screen_post_window() completed.");
      displayWindowBufferResult = 0;
    } else {
      log_message(LOG_ERROR, "displayWindowBuffer::screen_post_window() returned non-zero: %d", screenIfaceResult);
    }
  } else {
    log_message(LOG_ERROR, "displayWindowBuffer::screen_set_window_property_iv(SCREEN_PROPERTY_VISIBLE) returned non-zero: %d", screenIfaceResult);
  }

  return displayWindowBufferResult;
}

int bgrCreatePixmap(screen_context_t *pScreen_ctx, screen_pixmap_t *pScreen_pix) {
    const int pixmapUsage = SCREEN_USAGE_WRITE | SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE ;
	int createPixmapResult = -1;
	int screenIfaceResult;
	//screen_context_t screen_ctx = *pScreen_ctx;
	//screen_pixmap_t screen_pix = *pScreen_pix;

    screenIfaceResult = screen_create_pixmap(pScreen_pix, *pScreen_ctx);
    if (screenIfaceResult == EOK) {
    	screenIfaceResult = screen_set_pixmap_property_iv(*pScreen_pix, SCREEN_PROPERTY_USAGE, &pixmapUsage);
        if (screenIfaceResult == EOK) {
     	   screenIfaceResult = screen_set_pixmap_property_iv(*pScreen_pix, SCREEN_PROPERTY_FORMAT, (int[]){SCREEN_FORMAT_RGBX8888});
           if (screenIfaceResult == EOK) {
        	   createPixmapResult = 0; //EOK
           } else {
               log_message(LOG_ERROR, "bgrCreatePixmap::screen_set_pixmap_property_iv(SCREEN_PROPERTY_FORMAT) returned non-zero: %d ", screenIfaceResult);
               createPixmapResult = -3;
           }
        } else {
            log_message(LOG_ERROR, "bgrCreatePixmap::screen_set_pixmap_property_iv(SCREEN_PROPERTY_USAGE) returned non-zero: %d ", screenIfaceResult);
            createPixmapResult = -2;
        }
    } else {
        log_message(LOG_ERROR, "bgrCreatePixmap::screen_create_pixmap() returned non-zero: %d ", screenIfaceResult);
        createPixmapResult = -1;
    }

	return createPixmapResult;
}


int bgrResetTxtPixmapBuffer(bgrTxtPixmapData *pTxtPixmapData, int *pixmap_size, fr_grBufferProps *pBuffProps) {
  int createResult = -1;
  int screenIfaceResult;

  if (pTxtPixmapData->txtPixmapBufferState == eHandleUninit)
  {
    screenIfaceResult = screen_set_pixmap_property_iv(pTxtPixmapData->txtPixmap, SCREEN_PROPERTY_BUFFER_SIZE, pixmap_size);
    if (screenIfaceResult != EOK) {
      log_message(LOG_ERROR, "bgrResetTxtPixmapBuffer::screen_set_pixmap_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) returned non-zero: %d ", screenIfaceResult);
      createResult = -1;
    }
  } else {
    screenIfaceResult = screen_destroy_pixmap_buffer (pTxtPixmapData->txtPixmap);
    pTxtPixmapData->txtPixmapBufferState = eHandleUninit;
  }

  if (screenIfaceResult == EOK) {
    screenIfaceResult = screen_create_pixmap_buffer(pTxtPixmapData->txtPixmap);
    if (screenIfaceResult == EOK) {
      pTxtPixmapData->txtPixmapBufferState = eHandleValid;
      screenIfaceResult = screen_get_pixmap_property_pv(pTxtPixmapData->txtPixmap, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&(pTxtPixmapData->txtPixmapBuffer));
      if (screenIfaceResult == EOK) {
        screenIfaceResult = screen_get_buffer_property_pv(pTxtPixmapData->txtPixmapBuffer, SCREEN_PROPERTY_POINTER, &(pTxtPixmapData->pTxtPixmapBuffer));
        if (screenIfaceResult == EOK) {
          screenIfaceResult = screen_get_buffer_property_iv(pTxtPixmapData->txtPixmapBuffer, SCREEN_PROPERTY_STRIDE, &(pTxtPixmapData->txtPixmapBufferStride));
          if (screenIfaceResult == EOK) {
            pBuffProps->fr_pix_buf_data = pTxtPixmapData->pTxtPixmapBuffer;
            pBuffProps->fr_buf_size_x = pixmap_size[0];
            pBuffProps->fr_buf_size_y = pixmap_size[1];
            pBuffProps->fr_bpp = 4; //@fix: Should be in bgrTxtPixmapData or Screen/WindowData
            createResult = 0; //EOK
          } else {
            log_message(LOG_ERROR, "bgrResetTxtPixmapBuffer::screen_get_buffer_property_iv(SCREEN_PROPERTY_STRIDE) returned non-zero: %d ", screenIfaceResult);
            createResult = -5;
          }
        } else {
          log_message(LOG_ERROR, "bgrResetTxtPixmapBuffer::screen_get_buffer_property_pv(SCREEN_PROPERTY_POINTER) returned non-zero: %d ", screenIfaceResult);
          createResult = -4;
        }
      } else {
        log_message(LOG_ERROR, "bgrResetTxtPixmapBuffer::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE) returned non-zero: %d ", screenIfaceResult);
        createResult = -3;
      }
    } else {
      log_message(LOG_ERROR, "bgrResetTxtPixmapBuffer::screen_create_window_buffers(1) returned non-zero: %d ", screenIfaceResult);
      createResult = -2;
    }
  }


  return createResult;
}


void bgrGetEnvText(char *txtStr, int maxTxtSize) {
  char* envVarVal = NULL;
  const char envVarName[] = "BOOT_TEXT_STR";
  const char envVarErrorTxt[] = "Environment variable BOOT_TEXT_STR is not set!";

  envVarVal = getenv( envVarName );
  log_message(LOG_DEBUG, "getenv(%s) returned: %s.", envVarName, envVarVal);
  if( envVarVal != NULL ) {
    strncpy(txtStr, envVarVal, maxTxtSize );
  } else {
    strncpy(txtStr, envVarErrorTxt, maxTxtSize );
  }
}


// Returns 0 on success, <0 for errors, >0 for warnings
ePipeResult bgrGetFifoText(char *txtStr, int maxTxtSize, const char* appPipeName) {
  ePipeResult getFifoTextResult = ePipe_Error_Unknown;
  int fd = -1;
  char *pCtrlChar = NULL;
  size_t pipeBytesRead;
  const char txtSanitizer[] = {'\n', '\r', '\t','\0'};

  if (appPipeName != NULL) {
    fd = open(appPipeName, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
      if (errno == ENOENT) {
        log_message(LOG_DEBUG, "open(appPipeName) failed: No FIFO yet. ");
        snprintf(txtStr, maxTxtSize, "Named pipe fifo: \'%s\' not found.", appPipeName);
        getFifoTextResult = ePipe_Warning_Missing;
      } else {
        log_message(LOG_DEBUG, "open(appPipeName) failed to open. ");
        getFifoTextResult = ePipe_Error_Open;
      }
    } else {
      pipeBytesRead = read(fd, txtStr, maxTxtSize - 1);
      if (pipeBytesRead < 0) {
        txtStr[0]='\0';
        getFifoTextResult = ePipe_Error_Read;
      } else if (pipeBytesRead == 0) {
        txtStr[0]='\0';
        getFifoTextResult = ePipe_Warning_Empty;
      } else if ((pipeBytesRead < maxTxtSize - 1) || (txtStr[maxTxtSize - 1] == '\0')) {
        txtStr[pipeBytesRead]='\0';
        getFifoTextResult = ePipe_Success;
        pCtrlChar = strpbrk(txtStr, txtSanitizer);
        *pCtrlChar = '\0';
      } else {
        txtStr[pipeBytesRead]='\0';
        log_message(LOG_INFO, "Piped more that max bytes. Null terminate & return");
        getFifoTextResult = ePipe_Warning_Overflow;
      }

      close(fd);
    }
  }

  return getFifoTextResult;
}




int bgrInitScreenWindow(bgrScrWinContexts *pScrWinCtxt) {
  int screenIfaceResult = -1;

  log_message(LOG_DEBUG, "Create Context ...");
  screenIfaceResult = screen_create_context(&(pScrWinCtxt->scrCtx), pScrWinCtxt->scrFlags);
  if (screenIfaceResult != EOK) {
    log_message(LOG_ERROR, "screen_create_context() returned non-zero: %d.", screenIfaceResult);
    return -1;
  } else {
    log_message(LOG_DEBUG, "screen_create_context() completed.");
    pScrWinCtxt->scrCtxState = eHandleValid;

    screenIfaceResult = bgrCreateWindow(pScrWinCtxt);
    if (screenIfaceResult != EOK) {
      log_message(LOG_ERROR, "createWindow() returned non-zero: %d", screenIfaceResult);
      screen_destroy_context(pScrWinCtxt->scrCtx);
      pScrWinCtxt->scrCtxState = eHandleUninit;
      return -1;
    } else {
      log_message(LOG_DEBUG, "createWindow() completed.");
      pScrWinCtxt->scrWinState = eHandleValid;
      pScrWinCtxt->scrWinDirtyRect[0] = 0;
      pScrWinCtxt->scrWinDirtyRect[1] = 0;
      pScrWinCtxt->scrWinDirtyRect[2] = pScrWinCtxt->scrWinBufferSize[0];
      pScrWinCtxt->scrWinDirtyRect[3] = pScrWinCtxt->scrWinBufferSize[1];
    }
    log_message(LOG_INFO, "Screen parameters: Screen size:%d,%d, Buffer size:%d,%d", pScrWinCtxt->scrWinSize[0], pScrWinCtxt->scrWinSize[1], pScrWinCtxt->scrWinBufferSize[0], pScrWinCtxt->scrWinBufferSize[1]);
    screenIfaceResult = createWindowBuffers(&(pScrWinCtxt->scrWin), (void **)&(pScrWinCtxt->scrWinBuffer));
    if (screenIfaceResult != EOK) {
        log_message(LOG_ERROR, "createWindowBuffers (SCREEN_PROPERTY_SIZE) returned non-zero: %d", screenIfaceResult);
        screen_destroy_window(pScrWinCtxt->scrWin);
        pScrWinCtxt->scrWinState = eHandleUninit;
        screen_destroy_context(pScrWinCtxt->scrCtx);
        pScrWinCtxt->scrCtxState = eHandleUninit;
        return -1;
    } else {
        log_message(LOG_INFO, "createWindowBuffers() completed.");
    }

  }

  return screenIfaceResult;
}


int bgrBlitImagePixmap(bgrImgPixmapData *imgPxmpData, bgrScrWinContexts *pScrWinCtxt) {
  float img_aspect = 1280/768;
  float display_aspect = 1280/768;
  int dest_width;
  int dest_height;
  int dest_x = 0;
  int dest_y = 0;
  int screenIfaceResult;
  static int attribs[200] = {0};

  // Calculate aspect ratio and fit to display
  img_aspect = (float)(imgPxmpData->img.w) / imgPxmpData->img.h;
  display_aspect = (float)(pScrWinCtxt->scrWinBufferSize[0]) / pScrWinCtxt->scrWinBufferSize[1];
  log_message(LOG_INFO, "Aspect Ratio Data: .img.w:%d, .img.h:%d, img_aspect:%.2f, .scrWinSize[0]:%d, .scrWinSize[1]:%d, display_aspect:%.2f ", imgPxmpData->img.w, imgPxmpData->img.h, img_aspect, pScrWinCtxt->scrWinSize[0], pScrWinCtxt->scrWinSize[1], display_aspect );

  if (img_aspect > display_aspect) {  // Image is wider than display
    dest_width = pScrWinCtxt->scrWinBufferSize[0];
    dest_height = pScrWinCtxt->scrWinBufferSize[0] / img_aspect;
  } else {  // Image is taller than display
    dest_width = pScrWinCtxt->scrWinBufferSize[1] * img_aspect;
    dest_height = pScrWinCtxt->scrWinBufferSize[1];
  }

  dest_x = (pScrWinCtxt->scrWinBufferSize[0] - dest_width) / 2;
  dest_y = (pScrWinCtxt->scrWinBufferSize[1] - dest_height) / 2;
  log_message(LOG_INFO, "Blit Attribute Data: dest_x:%d, dest_y:%d, dest_width:%d, dest_height:%d ", dest_x, dest_y, dest_width, dest_height);

  // Set up the attributes for blitting an image
  setup_blit_attributes(0, 0, imgPxmpData->img.w, imgPxmpData->img.h,
            dest_x, dest_y, dest_width, dest_height,
            255, SCREEN_TRANSPARENCY_NONE, SCREEN_QUALITY_NICEST,
            attribs);


  log_message(LOG_DEBUG, "Image screen blit ...");
  screenIfaceResult = screen_blit(pScrWinCtxt->scrCtx, pScrWinCtxt->scrWinBuffer, imgPxmpData->imgPixmapBuffer, attribs);
  if ( screenIfaceResult != EOK) {
    log_message(LOG_ERROR, "screen_blit() returned non-zero: %d", screenIfaceResult);
  } else {
    log_message(LOG_INFO, "screen_blit() completed!!!");
  }

  return screenIfaceResult;
}


int bgrClearScreen( bgrScrWinContexts *pScrWinCtxt) {
  static int attribs[200] = {0};
  int screenIfaceResult = 0;

  setup_fill_attributes(0, 0, pScrWinCtxt->scrWinBufferSize[0], pScrWinCtxt->scrWinBufferSize[1],
            255, 0x000000, attribs);
  screenIfaceResult = screen_fill(pScrWinCtxt->scrCtx, pScrWinCtxt->scrWinBuffer, attribs);
  if ( screenIfaceResult != EOK) {
    log_message(LOG_ERROR, "screen_blit() returned non-zero: %d", screenIfaceResult);
  } else {
    log_message(LOG_INFO, "screen_blit() completed!!!");
  }

  return screenIfaceResult;
}


void bgrCleanupScrWinContexts (bgrScrWinContexts *pScrWinCtxt) {
  if (pScrWinCtxt->scrCtxState == eHandleValid) {
    screen_destroy_context(pScrWinCtxt->scrCtx);
    pScrWinCtxt->scrCtxState = eHandleUninit;
  }
  if (pScrWinCtxt->scrWinState == eHandleValid) {
    screen_destroy_window(pScrWinCtxt->scrWin);
    pScrWinCtxt->scrWinState = eHandleUninit;
  }
}

//@fix: Following 2 are begging to be abstracted
void bgrCleanupImgPxmpContexts (bgrImgPixmapData *imgPxmpData) {
  if (imgPxmpData->imgPixmapState == eHandleValid) {
    screen_destroy_pixmap(imgPxmpData->imgPixmap);
    imgPxmpData->imgPixmapState = eHandleUninit;
  }
}

void bgrCleanupTxtPxmpContexts (bgrTxtPixmapData *txtPxmpData) {
  if (txtPxmpData->txtPixmapState == eHandleValid) {
    screen_destroy_pixmap(txtPxmpData->txtPixmap);
    txtPxmpData->txtPixmapState = eHandleUninit;
  }
}


int bgrGetScreenDpi(bgrScrWinContexts *pScrWinCtxt) {
  int screenIfaceResult;
  int getDpiResult = -1;
  int screenSize_mm[2];
  int screenDiag_mm;
  int screenDiag_in;
  int screenRes[2];
  int screenDiag_pix;

  screenIfaceResult = screen_get_window_property_pv(pScrWinCtxt->scrWin , SCREEN_PROPERTY_DISPLAY, (void **)&(pScrWinCtxt->scrDisp));
  if (screenIfaceResult == EOK) {
    screenIfaceResult = screen_get_display_property_iv(pScrWinCtxt->scrDisp, SCREEN_PROPERTY_PHYSICAL_SIZE, screenSize_mm);
    if (screenIfaceResult == EOK) {
      screenDiag_mm = (int)round(sqrt(screenSize_mm[0] * screenSize_mm[0] + screenSize_mm[1] * screenSize_mm[1]));
      screenDiag_in = (screenDiag_mm * 10) / 254;
      if (screenDiag_in == 0) {
        pScrWinCtxt->scrDispDpi = 0;
        getDpiResult = 0;
      } else {
        screenIfaceResult = screen_get_display_property_iv(pScrWinCtxt->scrDisp, SCREEN_PROPERTY_SIZE, screenRes);
        if (screenIfaceResult == EOK) {
          screenDiag_pix = (int)round(sqrt(screenRes[0] * screenRes[0] + screenRes[1] * screenRes[1]));
          pScrWinCtxt->scrDispDpi = screenDiag_pix/screenDiag_in;
          getDpiResult = 0;
        } else {
          log_message(LOG_WARNING, "getScreenDpi::screen_get_window_property_pv(SCREEN_PROPERTY_SIZE) returned non-zero: %d", screenIfaceResult);
          getDpiResult = -3;;
        }
      }
    } else {
      log_message(LOG_WARNING, "getScreenDpi::screen_get_window_property_pv(SCREEN_PROPERTY_PHYSICAL_SIZE) returned non-zero: %d", screenIfaceResult);
      getDpiResult = -2;
    }
  } else {
    log_message(LOG_WARNING, "getScreenDpi::screen_get_window_property_pv(SCREEN_PROPERTY_DISPLAY) returned non-zero: %d", screenIfaceResult);
    getDpiResult = -1;
  }
  return getDpiResult;
}

int bgrGetDisplayText(eTextSources txtSrc, char *dispText, int dispTextSize, char* appPipeName) {
    int getTextResult = 0;

    switch (txtSrc) {
        case eTxtSrc_PARAM:
            if (getParamValueByIndex(PARAM_TEXT, PARAM_COUNT, params, dispText) != 0) {
                log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT) returned non-zero after (parse_arguments() == PARSE_SUCCESS)");
                getTextResult= -1;
            }
            if (strnlen(dispText, dispTextSize) == 0) {
                log_message(LOG_ERROR, "txtSrc=eTxtSrc_PARAM, but valid text size is zero.");
                getTextResult= -1;
            }
            break;
        case eTxtSrc_ENVVAR:
            bgrGetEnvText(dispText, dispTextSize);
            if (strnlen(dispText, dispTextSize) == 0) {
                log_message(LOG_ERROR, "txtSrc=eTxtSrc_ENVVAR, but valid text size is zero.");
                getTextResult= -1;
            }
            break;
        case eTxtSrc_PIPE:
            getTextResult = bgrGetFifoText(dispText, dispTextSize, appPipeName);
            if (getTextResult == ePipe_Success) {
                getTextResult = 0;
            } else if ((getTextResult > 0)) {
                //For FIFO treat warnings as success. These are empty or incomplete fifos. Worst case is a flicker of text.
                getTextResult = 0;
            } else {
                log_message(LOG_ERROR, "bgrGetFifoText() returned negative (error): %d", getTextResult);
                getTextResult= -1;
            }
            break;
        case eTxtSrc_NONE:
           default:
           break;
    }

    return getTextResult;
}


ePipeResult bgrGetFifoImageFileName(char *imageName, int imageNameSize, char* appImagePipeName) {
  ePipeResult getFifoTextResult = ePipe_Error_Unknown;
  int fd = -1;
  size_t pipeBytesRead;
  size_t strLen;
  char *pCtrlChar = NULL;
  const char txtSanitizer[] = {'\n', '\r','\0'};


  if (appImagePipeName != NULL) {
    fd = open(appImagePipeName, O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
      if (errno == ENOENT) {
        log_message(LOG_WARNING, "open(appPipeName) failed: No FIFO yet. ");
        imageName[0] = '\0';
        getFifoTextResult = ePipe_Warning_Missing;
      } else {
        log_message(LOG_WARNING, "open(appPipeName) failed to open. ");
        getFifoTextResult = ePipe_Error_Open;
      }
    } else {
      pipeBytesRead = read(fd, imageName, imageNameSize - 1);
      if (pipeBytesRead < 0) {
        log_message(LOG_ERROR, "read(appPipeName) returned negative bytes read. ");
        imageName[0]='\0';
        getFifoTextResult = ePipe_Error_Read;
      } else if (pipeBytesRead == 0) {
        log_message(LOG_WARNING, "read(appPipeName) returned 0 bytes read. ");
        getFifoTextResult = ePipe_Warning_Empty;
      } else if ((pipeBytesRead <= imageNameSize - 1)) {
        log_message(LOG_DEBUG, "read(appPipeName) got %d bytes. ", pipeBytesRead);
        imageName[pipeBytesRead]='\0';
        pCtrlChar = strpbrk(imageName, txtSanitizer);
        if (pCtrlChar != NULL) {
          *pCtrlChar = '\0';
        }
        strLen = strnlen(imageName,imageNameSize) ;
        if (strLen <= 0) {
          log_message(LOG_WARNING, "bgrGetFifoImageFileName::read(appPipeName) returned %d bytes read, but strnlen(imageName) returned zero.", pipeBytesRead);
          getFifoTextResult = ePipe_Warning_Empty;
        } else {
          getFifoTextResult = ePipe_Success;
        }
      } else {
        log_message(LOG_ERROR, "bgrGetFifoImageFileName::Unhandled case of read(pipe).");
        imageName[0]='\0';
        getFifoTextResult = ePipe_Error_Read;
      }

      close(fd);
    }
  }

  return getFifoTextResult;
}

int bgrGetImageName(eImageSources imgSrc, char *imgFileName, int imgFileNameSize, char *imgAppPipeName) {
    int getImgNameResult = 0;

    switch (imgSrc) {
    case eImgSrc_NONE:
        log_message(LOG_WARNING, "bgrGetImageName() called with ImageSource = NONE");
        getImgNameResult = -1;
        break;
    case eImgSrc_PIPE:
        if (bgrGetFifoImageFileName(imgFileName, imgFileNameSize, imgAppPipeName) != ePipe_Success) {
            log_message(LOG_WARNING, "bgrGetImageName() failed");
            getImgNameResult = -1;
        }
        break;
    case eImgSrc_PARAM:
        if (getParamValueByIndex(PARAM_IMG_FILE, PARAM_COUNT, params, imgFileName) != 0) {
            log_message(LOG_WARNING, "getParamValueByIndex(PARAM_FILE) returned non-zero after (parse_arguments() == PARSE_SUCCESS)");
            getImgNameResult = -1;
        }
        break;
    default:
        log_message(LOG_WARNING, "bgrGetImageName() called with unexpected image source. (Sync to validate parameter function");
        getImgNameResult = -1;
    }

    if (getImgNameResult == 0) {
        if (validate_img(imgFileName) != 1) {
            log_message(LOG_WARNING, "bgrGetImageName(): validate_img(%s) failed ", imgFileName);
            getImgNameResult = -1;
        } else {
            getImgNameResult = 0;
        }
    }

    return getImgNameResult;
}



int main(int argc, char *argv[])
{
  bgrScrWinContexts grWinCtxt;
  bgrImgPixmapData grImgPxmpData;
  bgrTxtPixmapData grTxtPxmpData;
  bgrOnScreenContentsData grOnScrConts;
  fr_grBufferProps ftGrBuffProps;


  static int attribs[200] = {0};
  char txtStr[PARAM_MAX_LENGTH];
  char txtAppPipeName[PARAM_MAX_LENGTH];
  char imgAppPipeName[PARAM_MAX_LENGTH];
  char tmpParamStr[PARAM_MAX_LENGTH];
  int screenIfaceResult = -1;
  int strWidth, strHeight, maxPenPos_y;
  unsigned int sleepTime = 200000;

  eImageSources imgSrc = eImgSrc_PARAM;
  eTextSources txtSrc = eTxtSrc_PARAM;


   log_init(LOG_DEFAULT);
   memset(attribs, SCREEN_BLIT_END, sizeof(attribs));
   memset(&ftGrBuffProps, 0, sizeof(fr_grBufferProps));
   memset(&grWinCtxt, 0, sizeof(grWinCtxt));
   memset(&grOnScrConts, 0, sizeof(grOnScrConts));
   grOnScrConts.onScr_imgFileName[0]='#'; //Set filenames to invalid not empty.
   grOnScrConts.onScr_ttfFileName[0]='#'; //Set filenames to invalid not empty.
   grWinCtxt.pScrWin_UserData = (void*)&grOnScrConts;
   memset(&grImgPxmpData, 0, sizeof(bgrImgPixmapData));

   //Generate default FIFO names for help and code. The code one will later be overwritten by parameters handling
   char *equals = strchr(argv[0], '/');
   if (equals == NULL) {
       // Add exact path in case it is missing. If launched by Momentics the path is missing in arg[0]
       snprintf(imgAppPipeName, sizeof(imgAppPipeName), "/tmp/%s_imgFifo", argv[0]);
       snprintf(txtAppPipeName, sizeof(txtAppPipeName), "/tmp/%s_txtFifo", argv[0]);
   } else {
       snprintf(imgAppPipeName, sizeof(imgAppPipeName), "%s_imgFifo", argv[0]);
       snprintf(txtAppPipeName, sizeof(txtAppPipeName), "%s_txtFifo", argv[0]);
   }
   if (strnlen(imgAppPipeName, sizeof(imgAppPipeName)) == 0) {
       log_message(LOG_ERROR, "Failed to generate image pipe name");
       return -1;
   }
   strncpy(imgFifoName, imgAppPipeName, sizeof(imgFifoName));
   if (strnlen(txtAppPipeName, sizeof(txtAppPipeName)) == 0) {
       log_message(LOG_ERROR, "Failed to generate text pipe name");
       return -1;
   }
   strncpy(txtFifoName, txtAppPipeName, sizeof(txtFifoName));

   // Parse arguments, validate, and use parameters
   if (PARAM_COUNT == (sizeof(params) / sizeof(tCmdOptionParam))) {
       ParseResult prsArgReslt = parse_arguments(argc, argv, PARAM_COUNT, params);
       if (prsArgReslt == PARSE_SUCCESS) {

           // Process Image properties: Source & FIFO name
           if (getParamValueByIndex(PARAM_IMG_SOURCE, PARAM_COUNT, params, tmpParamStr) == 0) {
               if ( strcmp(tmpParamStr, "NONE") == 0 ) {
                   log_message(LOG_INFO, "Image Source is set to NONE. Text only??");
                   grImgPxmpData.imgFileName[0] = '\0';
               } else if ( strcmp(tmpParamStr, "PARAM") == 0 ) {
                   imgSrc = eImgSrc_PARAM;
                   if (getParamValueByIndex(PARAM_IMG_FILE, PARAM_COUNT, params, grImgPxmpData.imgFileName) != 0) {
                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_FILE) returned non-zero after (parse_arguments() == PARSE_SUCCESS)");
                       print_usage("imgLib-imgLib", PARAM_COUNT, params);
                       return -1;
                   }
               } else if ( strcmp(tmpParamStr, "NPFIFO") == 0 ) {
                   imgSrc = eImgSrc_PIPE;
                   if (getParamValueByIndex(PARAM_IMG_FIFO, PARAM_COUNT, params, imgAppPipeName) == 0) {
                       log_message(LOG_INFO, "Configured for Image by FIFO, FIFO name: %s", imgAppPipeName);
                   } else {
                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_IMG_FIFO) found un-validated argument after (parse_arguments() == PARSE_SUCCESS)");
                       print_usage("imgLib-imgLib", PARAM_COUNT, params);
                       return -1;
                   }
               } else {
                   log_message(LOG_ERROR, "getParamValueByIndex(PARAM_IMG_SOURCE) found an unsupported value: %s", tmpParamStr);
                   print_usage("imgLib-imgLib", PARAM_COUNT, params);
                   return -1;
               }
           } else {
               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_IMG_SOURCE) failed");
               print_usage("imgLib-imgLib", PARAM_COUNT, params);
               return -1;
           }

           // Process arguments: Text source
           if (getParamValueByIndex(PARAM_TEXT_SOURCE, PARAM_COUNT, params, tmpParamStr) == 0) {
               if ( strcmp(tmpParamStr, "NONE") == 0 ) {
                   txtSrc = eTxtSrc_NONE;
               } else if ( strcmp(tmpParamStr, "PARAM") == 0 ) {
                   txtSrc = eTxtSrc_PARAM;
               } else if ( strcmp(tmpParamStr, "ENVVAR") == 0 ) {
                   txtSrc = eTxtSrc_ENVVAR;
               } else if ( strcmp(tmpParamStr, "NPFIFO") == 0 ) {
                   txtSrc = eTxtSrc_PIPE;
                   if (getParamValueByIndex(PARAM_TEXT_FIFO, PARAM_COUNT, params, txtAppPipeName) == 0) {
                       log_message(LOG_INFO, "Configured for Text by FIFO, FIFO name: %s", txtAppPipeName);
                   } else {
                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_IMG_FIFO) found un-validated argument after (parse_arguments() == PARSE_SUCCESS)");
                       print_usage("imgLib-imgLib", PARAM_COUNT, params);
                       return -1;
                   }
               } else {
                   log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_SOURCE) failed");
                   print_usage("imgLib-imgLib", PARAM_COUNT, params);
                   return -1;
               }
           } else {
               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_SOURCE) failed");
               print_usage("imgLib-imgLib", PARAM_COUNT, params);
               return -1;
           }


           // Set Screen/Window properties
           grWinCtxt.scrFlags = SCREEN_APPLICATION_CONTEXT;
           grWinCtxt.scrWinFormat = SCREEN_FORMAT_RGBX8888;
           grWinCtxt.scrWinUsage = SCREEN_USAGE_WRITE | SCREEN_USAGE_READ | SCREEN_USAGE_NATIVE | SCREEN_USAGE_ROTATION;
           if (getParamValueByIndex(PARAM_ROTATION, PARAM_COUNT, params, tmpParamStr) != 0) {
               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_ROTATION) returned non-zero after (parse_arguments() == PARSE_SUCCESS)");
               return -1;
           } else {
               tmpParamStr[3] = 0;
               grWinCtxt.scrWinRotation = atoi(tmpParamStr);
               if (grWinCtxt.scrWinRotation != 0 ) {
                   grWinCtxt.scrWinUsage |= SCREEN_USAGE_ROTATION;
               }
           }
           if (getParamValueByIndex(PARAM_SCALE, PARAM_COUNT, params, tmpParamStr) == 0) {
                   if ( strcmp(tmpParamStr, "NONE") == 0 ) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_NONE;
                   } else if ( strcmp(tmpParamStr, "STRETCH") == 0) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_STRETCH;
                   } else if ( strcmp(tmpParamStr, "ZOOM") == 0) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_ZOOM;
                   } else if ( strcmp(tmpParamStr, "FILL") == 0) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_FILL;
                   } else if ( strcmp(tmpParamStr, "SHIFT_UP") == 0) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_HALF_LINE_SHIFT_UP;
                   } else if ( strcmp(tmpParamStr, "SHIFT_DOWN") == 0) {
                       grWinCtxt.scrWinScaleMode = SCREEN_SCALE_HALF_LINE_SHIFT_DOWN;
                   } else {
                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_SCALE) returned unexpected value. (Check validation function)");
                       return -1;
                   }
           } else {
               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_SCALE) failed");
               return -1;
           }
           if (getParamValueByIndex(PARAM_MIRROR, PARAM_COUNT, params, tmpParamStr) == 0) {
                   if ( strcmp(tmpParamStr, "DISABLED") == 0 ) {
                       grWinCtxt.scrWinMirrorMode = SCREEN_MIRROR_DISABLED;
                   } else if ( strcmp(tmpParamStr, "NORMAL") == 0) {
                       grWinCtxt.scrWinMirrorMode = SCREEN_MIRROR_NORMAL;
                   } else if ( strcmp(tmpParamStr, "STRETCH") == 0) {
                       grWinCtxt.scrWinMirrorMode = SCREEN_MIRROR_STRETCH;
                   } else if ( strcmp(tmpParamStr, "ZOOM") == 0) {
                       grWinCtxt.scrWinMirrorMode = SCREEN_MIRROR_ZOOM;
                   } else if ( strcmp(tmpParamStr, "FILL") == 0) {
                       grWinCtxt.scrWinMirrorMode = SCREEN_MIRROR_FILL;
                   } else {
                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_MIRROR) returned unexpected value. (Check validation function)");
                       return -1;
                   }
           } else {
               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_MIRROR) failed");
               return -1;
           }

           // Init Screen, Screen Window, Window Buffer(s) ...
           screenIfaceResult = bgrInitScreenWindow(&grWinCtxt);
           if (screenIfaceResult != EOK) {
               log_message(LOG_ERROR, "bgrInitScreenWindow() returned non-zero: %d", screenIfaceResult);
               return -1;
           } else {
               log_message(LOG_INFO, "bgrInitScreenWindow() completed.");
           }

           // Init Image buffer
           screenIfaceResult = bgrCreatePixmap(&(grWinCtxt.scrCtx), &(grImgPxmpData.imgPixmap));
           if (screenIfaceResult != EOK) {
               log_message(LOG_ERROR, "createPixmap(screen_pix) returned non-zero: %d", screenIfaceResult);
               bgrCleanupScrWinContexts(&grWinCtxt);
               return -1;
           } else {
               log_message(LOG_INFO, "createPixmap(screen_pix) completed.");
           }

           //Init Text: Pixmap, buffer, Freetype, Font face.
           if (txtSrc != eTxtSrc_NONE) {
               memset(&grTxtPxmpData, 0, sizeof(bgrTxtPixmapData));
               if (getParamValueByIndex(PARAM_FONT_FILE, PARAM_COUNT, params, grTxtPxmpData.ttfFileName) != 0) {
                   log_message(LOG_ERROR, "getParamValueByIndex(PARAM_FONT) returned non-zero after (parse_arguments() == PARSE_SUCCESS)");
                   return -1;
               }
               log_message(LOG_DEBUG, "createPixmap(screen_pix_text) ...");
               screenIfaceResult = bgrCreatePixmap(&(grWinCtxt.scrCtx), &(grTxtPxmpData.txtPixmap));
               if (screenIfaceResult != EOK) {
                   log_message(LOG_ERROR, "createPixmap(screen_pix_text) returned non-zero: %d", screenIfaceResult);
                   bgrCleanupScrWinContexts(&grWinCtxt);
                   bgrCleanupImgPxmpContexts (&grImgPxmpData);
                   return -1;
               } else {
                   log_message(LOG_INFO, "createPixmap(screen_pix_text) completed.");
               }

               screenIfaceResult = bgrGetScreenDpi(&grWinCtxt);
               if ((screenIfaceResult != EOK) || (grWinCtxt.scrDispDpi == 0)) {
                   log_message(LOG_WARNING, "bgrGetScreenDpi() did not get DPI. Result: %d, DPI: %d", screenIfaceResult, grWinCtxt.scrDispDpi);
                   grWinCtxt.scrDispDpi = ftCalcDpi(TFT_WIDTH_MM, TFT_HEIGHT_MM, TFT_HORIZONTAL_RESOLUTION, TFT_VERTICAL_RESOLUTION);
               }
               if ( (grWinCtxt.scrDispDpi > 200) || (grWinCtxt.scrDispDpi < 50)) {
                   log_message(LOG_WARNING, "DPI of %d is suspicious.", grWinCtxt.scrDispDpi);
               }

               if (getParamValueByIndex(PARAM_FONT_SIZE, PARAM_COUNT, params, tmpParamStr) == 0) {
                   grTxtPxmpData.fontSize = atoi(tmpParamStr);
                   if ((grTxtPxmpData.fontSize < FONT_SIZE_MIN) || (grTxtPxmpData.fontSize > FONT_SIZE_MAX)) {
                       log_message(LOG_ERROR, "Font Size out of bounds. Min: %d, Max: %s, Actual: %d", FONT_SIZE_MIN, FONT_SIZE_MAX, grTxtPxmpData.fontSize);
                       return -1;
                   }
               } else {
                   log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_HPOS) failed");
                   return -1;
               }


               screenIfaceResult = ftInitFont(grTxtPxmpData.ttfFileName, grTxtPxmpData.fontSize, grWinCtxt.scrDispDpi);
               if (screenIfaceResult != fr_OK) {
                   log_message(LOG_ERROR, "ftInitFont(ttfFileName:%s, FontSize:%d, dpi:%d) returned non-zero: %d", grTxtPxmpData.ttfFileName, grTxtPxmpData.fontSize, grWinCtxt.scrDispDpi, screenIfaceResult);
                   bgrCleanupScrWinContexts(&grWinCtxt);
                   bgrCleanupImgPxmpContexts (&grImgPxmpData);
                   bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                   return -1;
               } else {
                   log_message(LOG_INFO, "ftInitFont() completed.");
               }
           }

           //Render loop
           while (1) {
               // Did anything change? Check what needs update/clear/redraw
               bgrOnScreenContentsData *pTmpScrCntData = (bgrOnScreenContentsData*)(grWinCtxt.pScrWin_UserData);
               int GetImageNameResult = bgrGetImageName(imgSrc, grImgPxmpData.imgFileName, sizeof(grImgPxmpData.imgFileName), imgAppPipeName);
               if ( (GetImageNameResult == 0)
                     && (strncmp(grImgPxmpData.imgFileName, pTmpScrCntData->onScr_imgFileName,
                                min(sizeof(grImgPxmpData.imgFileName), sizeof(pTmpScrCntData->onScr_textString))) != 0)) {
                   //Image is valid AND is different to current on screen;
                   grImgPxmpData.imgRedrawNeeded = 1;
               }
               int GetTextResult = bgrGetDisplayText(txtSrc, txtStr, sizeof(txtStr), txtAppPipeName);
               if ( (txtSrc != eTxtSrc_NONE)
                       && (GetTextResult == 0)
                       /*&& (strnlen(txtStr, sizeof(txtStr)) != 0)*/
                       && (strncmp(txtStr, pTmpScrCntData->onScr_textString, sizeof(txtStr)) != 0 )
                       ) {
                   grTxtPxmpData.txtRedrawNeeded = 1;
               }

               // There are no separate layers so if anything changes force redraw of everything to fix background.
               if ((grImgPxmpData.imgRedrawNeeded != 0) || (grTxtPxmpData.txtRedrawNeeded != 0)) {
                   grImgPxmpData.imgRedrawNeeded = 1;
                   grTxtPxmpData.txtRedrawNeeded = 1;
               }

               // Render image
               if (grImgPxmpData.imgRedrawNeeded != 0) {
                   // Clear screen. @todo: Needed only in case new image doesn't cover full screen or same area as previous one.
                   screenIfaceResult = bgrClearScreen(&grWinCtxt);
                   if ( screenIfaceResult != 0 ) {
                       log_message(LOG_ERROR, "bgrClearScreen() failed.");
                       return -1;
                   }
                   //If valid image then display it. Else erase the whole screen
                   if (GetImageNameResult == 0) {
                       //if image is valid, has it changed? Call imgLoadFile only if a new image needed for the pixmap.
                       if (strncmp(grImgPxmpData.imgFileName, pTmpScrCntData->onScr_imgFileName,
                               min(sizeof(grImgPxmpData.imgFileName), sizeof(pTmpScrCntData->onScr_textString))) != 0) {
                           screenIfaceResult =  bgrLoadImagePixmap(&grImgPxmpData);
                           if (screenIfaceResult != EOK) {
                               log_message(LOG_ERROR, "bgrLoadImagePixmap() returned non-zero: %d", screenIfaceResult);
                               bgrCleanupScrWinContexts(&grWinCtxt);
                               bgrCleanupImgPxmpContexts (&grImgPxmpData);
                               bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                               return -1;
                           } else {
                               log_message(LOG_INFO, "bgrLoadImagePixmap(screen_pix) completed.");
                               strncpy(pTmpScrCntData->onScr_imgFileName, grImgPxmpData.imgFileName, sizeof(pTmpScrCntData->onScr_imgFileName));
                           }
                       }
                       log_message(LOG_DEBUG, "Image pixmap screen blit ...");
                       screenIfaceResult =  bgrBlitImagePixmap(&grImgPxmpData, &grWinCtxt);
                       if (screenIfaceResult != EOK) {
                           log_message(LOG_ERROR, "bgrBlitImagePixmap(imgPxmp) returned non-zero: %d", screenIfaceResult);
                           bgrCleanupScrWinContexts(&grWinCtxt);
                           bgrCleanupImgPxmpContexts (&grImgPxmpData);
                           bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                           return -1;
                       } else {
                           log_message(LOG_INFO, "bgrBlitImagePixmap(imgPxmp) completed.");
                           grImgPxmpData.imgRedrawNeeded = 0;
                           grWinCtxt.scrRedrawNeeded = 1;
                       }
                   } else {
                       log_message(LOG_INFO, "No image by bgrGetImageName() so just clear redraw flag.");
                           grImgPxmpData.imgRedrawNeeded = 0;
                           grWinCtxt.scrRedrawNeeded = 1;
                   }
               }

               // Render text.
               if (grTxtPxmpData.txtRedrawNeeded != 0) {
                   if (txtSrc != eTxtSrc_NONE) {
                       if ((screenIfaceResult == 0) && (strnlen(txtStr, sizeof(txtStr)) != 0)) {
                           // Did the string change? If yes render new one before blit. Either case blit as image might have changed.
                           if (strncmp(txtStr, pTmpScrCntData->onScr_textString, sizeof(txtStr)) != 0 ) {
                               screenIfaceResult = bgrResetTxtPixmapBuffer(&grTxtPxmpData, grWinCtxt.scrWinSize, &ftGrBuffProps);

                               log_message(LOG_DEBUG, "frCalcStrPixelSize() for text:%s ", txtStr);
                               frCalcStrPixelSize(&maxPenPos_y, &strWidth, &strHeight, txtStr);
                               if ((strWidth < 1) || (strHeight < 1)) {
                                 log_message(LOG_WARNING, "frCalcStrPixelSize() returned strWidth:%d, strHeight:%d", strWidth, strHeight);
                                 grTxtPxmpData.txtRedrawNeeded = 0;
                               } else {
                                   // Handle Horizontal Alignment
                                   if (getParamValueByIndex(PARAM_TEXT_HPOS, PARAM_COUNT, params, tmpParamStr) == 0) {
                                           if ( strcmp(tmpParamStr, "LEFT") == 0 ) {
                                               log_message(LOG_INFO, "Test left adjusted. Vals: strWidth: %d, grWinCtxt.scrWinSize[2]: %d", strWidth, grWinCtxt.scrWinSize[2]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x = 0;
                                           } else if ( strcmp(tmpParamStr, "RIGHT") == 0) {
                                               log_message(LOG_INFO, "Test right adjusted. Vals: strWidth: %d, grWinCtxt.scrWinSize[2]: %d", strWidth, grWinCtxt.scrWinSize[2]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x = grWinCtxt.scrWinSize[2] - min(0 + strWidth, grWinCtxt.scrWinSize[2]);
                                           } else if ( strcmp(tmpParamStr, "CENTER") == 0) {
                                               log_message(LOG_INFO, "Test center adjusted. Vals: strWidth: %d, grWinCtxt.scrWinSize[2]: %d", strWidth, grWinCtxt.scrWinSize[2]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x = (grWinCtxt.scrWinSize[2] - min(0 + strWidth, grWinCtxt.scrWinSize[2])) / 2;
                                           } else {
                                               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_HPOS) returned unexpected value. (Check validation function)");
                                               return -1;
                                           }
                                   } else {
                                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_HPOS) failed");
                                       return -1;
                                   }
                                   grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_width = min(0 + strWidth, grWinCtxt.scrWinSize[2]);
                                   //Text taller than screen/window could be made to work through proper bounding box calculations, but is not worth it.
                                   if (grWinCtxt.scrWinSize[1] <= strHeight) {
                                       log_message(LOG_ERROR, "Text too bid. grWinCtxt.scrWinSize[1]=%d <= strHeight=%d", grWinCtxt.scrWinSize[1], strHeight);
                                       return -1;
                                   }
                                   // Handle Vertical Alignment
                                   if (getParamValueByIndex(PARAM_TEXT_VPOS, PARAM_COUNT, params, tmpParamStr) == 0) {
                                           if ( strcmp(tmpParamStr, "TOP") == 0 ) {
                                               log_message(LOG_INFO, "Test top adjusted. Vals: strHeight: %d, grWinCtxt.scrWinSize[1]: %d", strHeight, grWinCtxt.scrWinSize[1]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y = 0;
                                           } else if ( strcmp(tmpParamStr, "BOTTOM") == 0) {
                                               log_message(LOG_INFO, "Test bottom adjusted. Vals: strHeight: %d, grWinCtxt.scrWinSize[1]: %d", strHeight, grWinCtxt.scrWinSize[1]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y = grWinCtxt.scrWinSize[1] - strHeight - 1;
                                           } else if ( strcmp(tmpParamStr, "CENTER") == 0) {
                                               log_message(LOG_INFO, "Test center adjusted vertically. Vals: strHeight: %d, grWinCtxt.scrWinSize[1]: %d", strHeight, grWinCtxt.scrWinSize[1]);
                                               grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y = (grWinCtxt.scrWinSize[1] - strHeight - 1) / 2;
                                           } else {
                                               log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_VPOS) returned unexpected value. (Check validation function)");
                                               return -1;
                                           }
                                   } else {
                                       log_message(LOG_ERROR, "getParamValueByIndex(PARAM_TEXT_VPOS) failed");
                                       return -1;
                                   }
                                   grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_height = strHeight;
                                   grTxtPxmpData.ftCanvasProps.penPos.pen_x = 0 << 6;
                                   grTxtPxmpData.ftCanvasProps.penPos.pen_y = maxPenPos_y << 6;
                                   log_message(LOG_DEBUG, "bb_start_x:%d, bb_start_y:%d, bb_width:%d, bb_height:%d, pen_x:%d, pen_y:%d", grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x, grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y, grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_width, grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_height, grTxtPxmpData.ftCanvasProps.penPos.pen_x, grTxtPxmpData.ftCanvasProps.penPos.pen_y);
                                   //Render text into pixmap buffer
                                   screenIfaceResult = ftRender(ftGrBuffProps, &(grTxtPxmpData.ftCanvasProps), txtStr);
                                   if ( screenIfaceResult != fr_OK) {
                                   log_message(LOG_ERROR, "ftRender() returned non-zero: %d", screenIfaceResult);
                                     bgrCleanupScrWinContexts(&grWinCtxt);
                                     bgrCleanupImgPxmpContexts (&grImgPxmpData);
                                     bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                                     return -1;
                                   } else {
                                     log_message(LOG_INFO, "ftRender() completed!!!");
                                   }
                               }
                               // Store the new string as current in the window context user data.
                               strncpy(pTmpScrCntData->onScr_textString, txtStr, sizeof(txtStr));
                           }

                           // Set up the attributes for blitting the text pixmap
                           if (grTxtPxmpData.txtRedrawNeeded != 0) {
                               setup_blit_attributes(grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x,    /*src_x*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y,    /*src_y*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_width,      /*src_width*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_height,     /*src_height*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_x,    /*dest_x*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_start_y,    /*dest_y*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_width,      /*dest_width*/
                                                     grTxtPxmpData.ftCanvasProps.txtBoundBox.bb_height,     /*dest_height*/
                                                     255,                       /*global alpha*/
                                                     SCREEN_TRANSPARENCY_NONE,
                                                     SCREEN_QUALITY_NICEST,
                                                     attribs);

                               log_message(LOG_DEBUG, "Text pixmap screen blit ...");
                               screenIfaceResult = screen_blit(grWinCtxt.scrCtx, grWinCtxt.scrWinBuffer, grTxtPxmpData.txtPixmapBuffer, attribs);
                               if ( screenIfaceResult != EOK) {
                                 log_message(LOG_ERROR, "screen_blit() returned non-zero: %d", screenIfaceResult);
                                 bgrCleanupScrWinContexts(&grWinCtxt);
                                 bgrCleanupImgPxmpContexts (&grImgPxmpData);
                                 bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                                 return -1;
                               } else {
                                 log_message(LOG_INFO, "screen_blit() for text completed!!!");
                                 grTxtPxmpData.txtRedrawNeeded = 0;
                                 grWinCtxt.scrRedrawNeeded = 1;
                               }

                           }
                       } else {
                           log_message(LOG_INFO, "Text Redraw requested, but string is not valid. Clear flag.");
                           grTxtPxmpData.txtRedrawNeeded = 0;
                       }
                   } else {
                       log_message(LOG_INFO, "Text Redraw requested, but image source is NONE. Clear flag.");
                       grTxtPxmpData.txtRedrawNeeded = 0;
                   }
               }

               if (grWinCtxt.scrRedrawNeeded != 0) {
                   // After pixmaps blitted to the screen buffer do this to make anything change. And yes, every time even if not double buffered.
                   log_message(LOG_DEBUG, "displayWindowBuffer() ...");
                   screenIfaceResult = displayWindowBuffer(&(grWinCtxt.scrWin), grWinCtxt.scrWinBuffer, grWinCtxt.scrWinDirtyRect);
                   if ( screenIfaceResult != EOK) {
                       log_message(LOG_ERROR, "displayWindowBuffer() returned non-zero: %d", screenIfaceResult);
                       bgrCleanupScrWinContexts(&grWinCtxt);
                       bgrCleanupImgPxmpContexts (&grImgPxmpData);
                       bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
                      return -1;
                   } else {
                       log_message(LOG_INFO, "displayWindowBuffer() completed!!!");
                       grWinCtxt.scrRedrawNeeded = 0;
                   }
               }

               log_message(LOG_DEBUG, "Sleep for %d uS", sleepTime);
               usleep(sleepTime);
           }

           // @todo: Move this in the loop and replace it with FIFO maybe. For now slay has same effect with less effort.
           printf("Press Enter to exit...\n");
           getchar();

           // Clean up
           log_message(LOG_DEBUG, "Free img, window, context...");
           //free(img.access.direct.data); //img_destroy(&img);
           bgrCleanupScrWinContexts(&grWinCtxt);
           bgrCleanupImgPxmpContexts (&grImgPxmpData);
           bgrCleanupTxtPxmpContexts (&grTxtPxmpData);
       } else {
           log_message(LOG_ERROR, "Params parse failed!");

           switch (prsArgReslt) {
           case PARSE_ERROR_MEMORY:
           case PARSE_ERROR_INVALID_FORMAT:
           case PARSE_ERROR_DUPLICATE_VALUE:
           case PARSE_ERROR_UNKNOWN:
           case PARSE_ERROR_INVALID_VALUE:
           case PARSE_ERROR_MISSING:
               printf("Err: Arguments parsing failed for %s\n", get_parse_error_string(prsArgReslt));
               break;
           default:
               printf("Err: Arguments parsing failed for unknown reason\n\n");
           }

           print_usage("imgLib-imgLib", PARAM_COUNT, params);

           return -1;
       }
   } else {
       log_message(LOG_ERROR, "Parameters enumeration and array sizes do not match! (ImgLib.c: PARAM_COUNT:%d, Array:%d\n)", PARAM_COUNT,sizeof(params) / sizeof(tCmdOptionParam));
       return -1;
   }

 return 0;
}
