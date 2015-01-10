#ifndef REGEXW_H
#define REGEXW_H

#include "util.h"

typedef struct ps_regex_t
{
    char* regex;
    char* error;
    unsigned int found;
} ps_regex_t;


/*
 * Create a ps_regex_t type
 */
ps_status_t
ps_regex_create(ps_regex_t** regex);

ps_status_t
ps_regex_find(ps_regex_t* regex);

#endif
