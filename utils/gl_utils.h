/******************************************************************************\
\******************************************************************************/
#ifndef _GL_UTILS_H_
#define _GL_UTILS_H_

#include "gl_utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define GL_LOG_FILE "gl.log"
#define MAX_SHADER_LENGTH 262144

bool parse_file_into_str( const char *file_name, char *shader_str, int max_len );

#endif
