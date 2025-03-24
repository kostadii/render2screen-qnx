/**
 ******************************************************************************
 *
 *  @file Arc_main.c
 *
 *  @brief Implementation of simple logging for console Linux/UNIX/QNX system.
 *
 *
 ******************************************************************************
*/

/******************************************************************************
  Depends
 ******************************************************************************/
#include "logger.h"


/******************************************************************************
  Macro Definitions
 ******************************************************************************/

/******************************************************************************
  Type Definitions
 ******************************************************************************/

/******************************************************************************
  Constant Definitions
 ******************************************************************************/
const char *level_strings[LOG_UBOUND] = {
    "",
	"",
	"ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};

/******************************************************************************
  File Scope Variables
 ******************************************************************************/
log_level_t verbosity_level = LOG_UNINIT;

/******************************************************************************
  File Scope Function Prototypes
 ******************************************************************************/


void log_init(log_level_t set_level) {
    if ( !((set_level == LOG_DEFAULT) || (set_level == LOG_ERROR) || (set_level == LOG_WARNING) || (set_level == LOG_INFO) || (set_level == LOG_DEBUG)) ) {
        fprintf(stderr, "Improper use of log_init(): set_level is invalid: %d. ", set_level);
    } else {
    	verbosity_level = set_level;
    }
}


// Logs a message based on system Verbosity
//
void log_message(log_level_t message_level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    bool print_msg = false;

    if ((verbosity_level == LOG_UNINIT) || (verbosity_level >= LOG_UBOUND)) {
    	fprintf(stderr, "Improper use of log_message(): verbosity_level is %d. Was log_init not called?", verbosity_level);
    } else {
        if ( !((message_level == LOG_DEFAULT) || (message_level == LOG_ERROR) || (message_level == LOG_WARNING) || (message_level == LOG_INFO) || (message_level == LOG_DEBUG)) ) {
            fprintf(stderr, "Improper use of log_message(): message_level is incorrect: %d. ", message_level);
        } else {

        	// Error messages always go to stderr output.
        	if (message_level == LOG_ERROR) {
    		    fprintf(stderr, "[%s] ", level_strings[message_level]);
    		    vfprintf(stderr, format, args);
    	        fprintf(stderr, "\n");
        	}

        	//Print the message in stdout if message severity is on or above verbosity level
            switch (verbosity_level) {
            case (LOG_DEBUG):
                //In DEBUG level any message is printed in stdout
		        print_msg = true;
                break;
            case (LOG_INFO):
            	if ((message_level == LOG_ERROR) || (message_level == LOG_WARNING) || (message_level == LOG_INFO)) {
    		        print_msg = true;
            	}
                break;
            case (LOG_WARNING):
                if ((message_level == LOG_ERROR) || (message_level == LOG_WARNING)) {
            	    print_msg = true;
                }
                break;
            case (LOG_ERROR):
                if ((message_level == LOG_ERROR)) {
                    print_msg = true;
                }
                break;
            case (LOG_DEFAULT):
				//In DEFAULT Err goes to stderr, nothing else in stdout
                print_msg = false;
                break;
            default:
                print_msg = false;
                break;
            }

            if (print_msg == true) {
                fprintf(stdout, "[%s] ", level_strings[message_level]);
		        vfprintf(stdout, format, args);
	            fprintf(stdout, "\n");
            }
        }
    }
    va_end(args);
}
