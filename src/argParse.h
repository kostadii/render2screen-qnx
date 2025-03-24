/*
 * argParse.h
 *
 *  Created on: Dec 11, 2024
 *      Author: TBC
 */


// argParse.h
#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <stdbool.h>  // For boolean type
#include <stdio.h>    // For printing errors and usage info

#define PARAM_MAX_LENGTH 255

// Structure definition for command-line parameters
typedef struct {
    const char *option;              // Command-line option (e.g., "-file")
    char value[PARAM_MAX_LENGTH];    // Value passed after "="
    int (*validate)(const char *);   // Validation function pointer
    const char *option_format;       // Command-line option exact format
    const char *usage_info;          // Usage info for the argument
    const bool mandatory;            // Is the parameter required?
    bool was_passed;                 // Was the parameter passed?
    const char *default_value;       // Default value for the parameter
} tCmdOptionParam;

// Enumeration for return values from parse_arguments
typedef enum {
    PARSE_SUCCESS = 0,           // Success
    PARSE_ERROR_MEMORY,          // Memory allocation failure
    PARSE_ERROR_INVALID_FORMAT,  // Invalid argument format
    PARSE_ERROR_DUPLICATE_VALUE, // Duplicated value
    PARSE_ERROR_UNKNOWN,         // Unknown argument
    PARSE_ERROR_INVALID_VALUE,   // Invalid value for parameter
    PARSE_ERROR_MISSING,         // Missing mandatory argument
	PARSE_ERROR_UBOUND           // For bounds checking
} ParseResult;

// Function prototypes
ParseResult parse_arguments(int argc, char *argv[], const int paramc, tCmdOptionParam *params);
void print_usage(const char *program_name, int param_count, const tCmdOptionParam *params);
int getParamValueByIndex(int paramIndex, const int paramc, const tCmdOptionParam *params, char *paramVal);
const char *get_parse_error_string(ParseResult result);

// Validation functions
int validate_file(const char *value);
int validate_rotation(const char *value);
int validate_scale(const char *value);

#endif // ARGPARSE_H
