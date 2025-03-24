/*
 * logger.h
 *
 *  Created on: Jan 6, 2025
 *      Author: TBD
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>  // For boolean type
#include <stdio.h>
#include <stdarg.h>

typedef enum {
    LOG_UNINIT 		= 0,
	LOG_DEFAULT 	= 1,
	LOG_ERROR 		= 2,
    LOG_WARNING 	= 3,
    LOG_INFO 		= 4,
    LOG_DEBUG 		= 5,
	LOG_UBOUND 		= 6
} log_level_t;

void log_init(log_level_t set_level);
void log_message(log_level_t message_level, const char *format, ...);

#endif // LOGGER_H
