#ifndef REGEXW_H
#define REGEXW_H
#include <pcre.h>

#include "util.h"

#define MAX_REGEX_RESULT_OFFSET ( 50 )

typedef struct ps_regex_t
{
    char* regex;
    pcre* regex_compiled;
    pcre_extra* pcre_extra;
    int result_offset[MAX_REGEX_RESULT_OFFSET];
    const char* error;
    int error_offset;
    int error_code;
    ps_bool_t found;
} ps_regex_t;


/*
 * Create a ps_regex_t type
 */
ps_status_t
ps_regex_create(ps_regex_t** re, char* regex);

ps_status_t
ps_regex_find(ps_regex_t* re,
              const char* content,
              unsigned int content_offset);

ps_status_t
ps_regex_free(ps_regex_t* re);

char*
ps_regex_error_code_to_str(ps_regex_t* re);

#endif
