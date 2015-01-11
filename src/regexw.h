#ifndef REGEXW_H
#define REGEXW_H
#include <pcre.h>

#include "util.h"

typedef struct ps_regex_t
{
    char* regex;
    pcre* regex_compiled;
    pcre_extra* pcre_extra;
    const char* error;
    int error_offset;
    unsigned int found;
} ps_regex_t;


/*
 * Create a ps_regex_t type
 */
ps_status_t
ps_regex_create(ps_regex_t** re, char* regex);

ps_status_t
ps_regex_find(ps_regex_t* re);

#endif
