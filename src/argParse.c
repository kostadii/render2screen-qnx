/*
 * argParse.c
 *
 *  Created on: Dec 11, 2024
 *      Author: TBC
 */

// argParse.c
#include "logger.h"
#include "argParse.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


const char* parseErrStrings[PARSE_ERROR_UBOUND] = {

	"Success",							//PARSE_SUCCESS
	"Memory allocation failure",		//PARSE_ERROR_MEMORY
	"Invalid argument format",			//PARSE_ERROR_INVALID_FORMAT
	"Duplicated value",					//PARSE_ERROR_DUPLICATE_VALUE
	"Unknown argument",					//PARSE_ERROR_UNKNOWN
	"Invalid value for argument",		//PARSE_ERROR_INVALID_VALUE
	"Missing mandatory argument",		//PARSE_ERROR_MISSING
};


ParseResult parse_arguments(int argc, char *argv[], const int paramc, tCmdOptionParam *params) {
    log_message(LOG_DEBUG, "Enter parse_arguments");

    ParseResult result = PARSE_SUCCESS;

    int i = 1;
    while ((i < argc) && (result == PARSE_SUCCESS)) {
    	log_message(LOG_DEBUG, "Processing argument: %s", argv[i]);
        // Create a writable copy of the current argument
        char *arg_copy = strdup(argv[i]);
        if (!arg_copy) {
        	log_message(LOG_ERROR, "Memory allocation failed for argument %s", argv[i]);
            result = PARSE_ERROR_MEMORY;
        } else {
            // Find the '=' character in the argument
            char *equals = strchr(arg_copy, '=');
            if (!equals || equals == arg_copy) {
            	log_message(LOG_WARNING, "Invalid argument format: %s", argv[i]);
                result = PARSE_ERROR_INVALID_FORMAT;
            } else {
                // Split the argument into key and value
                *equals = '\0';
                const char *key = arg_copy;
                const char *value = equals + 1;

                log_message(LOG_INFO, "Key: %s, Value: %s", key, value);

                // Match the argument key with parameters
                int found = 0;
                int j = 0;
                do {
                    if (strcmp(key, params[j].option) == 0) {
                        found = 1;

                        if (params[j].was_passed) {
                        	log_message(LOG_WARNING, "Duplicate argument detected: %s", key);
                            result = PARSE_ERROR_DUPLICATE_VALUE;
                        } else {
                            if (params[j].validate && !params[j].validate(value)) {
                            	log_message(LOG_WARNING, "Invalid value for %s: %s", key, value);
                                result = PARSE_ERROR_INVALID_VALUE;
                            } else {
                                if (result == PARSE_SUCCESS) {
                                    strncpy(params[j].value, value, sizeof(params[j].value) - 1);
                                    params[j].value[sizeof(params[j].value) - 1] = '\0';
                                    params[j].was_passed = 1;
                                	log_message(LOG_DEBUG, "Processed key %s: value:%s assigned to params[%d]", key, value, j);
                                }
                            }
                        }
                    }
                    j++;
                } while ((j < paramc) && (!found || result != PARSE_SUCCESS));

                if (!found) {
                	log_message(LOG_WARNING, "Unknown argument: %s", key);
                    result = PARSE_ERROR_UNKNOWN;
                }
            }

            if (arg_copy) {
                free(arg_copy);
            }
        }
        i++;
    }
    log_message(LOG_DEBUG, "Arguments loop done.");

    // Check for missing mandatory parameters
    for (int j = 0; j < paramc; j++) {
        if (params[j].mandatory && !params[j].was_passed) {
        	log_message(LOG_WARNING, "Missing mandatory argument: %s", params[j].usage_info);
            result = PARSE_ERROR_MISSING;
        }
    }

    return result;
}


// Print usage information for all parameters
void print_usage(const char *program_name, int param_count, const tCmdOptionParam *params) {
    printf("\nUsage: %s ", program_name);

    for (int i = 0; i < param_count; i++) {
        printf(" %s", params[i].option_format);
    }

    printf("\n\n");

    for (int i = 0; i < param_count; i++) {
        printf(" %-12s\t%s \n", params[i].option, params[i].usage_info);
    }
    printf("\nAdditionally, check: use %s \n\n", program_name);
}

int getParamValueByIndex(int paramIndex, const int paramc, const tCmdOptionParam *params, char *paramVal) {
    int result = 1;
    //fprintf(stderr, "getParamValueByIndex: ParamIndex:%d paramc:%d\n ", paramIndex, paramc);
    log_message(LOG_DEBUG, "getParamValueByIndex: ParamIndex:%d paramc:%d", paramIndex, paramc);

    if ((paramIndex >= 0) && (paramIndex <= paramc)) {
        if (params[paramIndex].was_passed) {
          strncpy(paramVal, params[paramIndex].value,  sizeof(params[paramIndex].value) - 1);
          result = 0;
        } else {
          strncpy(paramVal, params[paramIndex].default_value,  PARAM_MAX_LENGTH - 1);
          result = 0;
        }
    }

    return result;
}




const char *get_parse_error_string(ParseResult result) {
	const char* strPtr = NULL;

	if ( result <= PARSE_ERROR_UBOUND ) {
	    log_message(LOG_DEBUG, "get_parse_error_string() called for: %d", result);
		strPtr = parseErrStrings[result];
	} else {
	    log_message(LOG_WARNING, "Out of bound request to get_parse_error_string(): called for %d, with UBOUND %d", result, PARSE_ERROR_UBOUND);
	}

	return strPtr;
}
