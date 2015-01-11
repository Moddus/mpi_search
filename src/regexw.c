#include <pcre.h>

#include "regexw.h"
#include "util.h"
#include "log.h"

ps_status_t
ps_regex_create(ps_regex_t** re, char* regex)
{
    ps_status_t rv = PS_SUCCESS;

    PS_MALLOC(*re, sizeof(ps_regex_t));
    (*re)->regex = regex;
    (*re)->error_offset = 0;
    (*re)->regex_compiled = pcre_compile((*re)->regex, 0, &(*re)->error, &(*re)->error_offset, NULL);
    (*re)->found = FALSE;

    return rv;

error:
    log_err("could not create ps_regex_t. error: %d\n", rv);
    return rv;
}

ps_status_t
ps_regex_find(ps_regex_t* re)
{
    pcre_free(re->regex_compiled);
    if(re->pcre_extra != NULL)
    {
        pcre_free(re->pcre_extra);
    }
    free(re);
    re = NULL;

    return PS_SUCCESS;
}
