#include <pcre.h>
#include <string.h>

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
    (*re)->regex_compiled = pcre_compile((*re)->regex, 0,
                                         &(*re)->error,
                                         &(*re)->error_offset, NULL);
    (*re)->found = FALSE;

    if((*re)->regex_compiled == NULL)
    {
        rv = PS_ERROR_OBJ_CREATE;
        goto error;
    }

    (*re)->pcre_extra = pcre_study( (*re)->regex_compiled,
                                    0,
                                    &(*re)->error);

    if((*re)->pcre_extra == NULL)
    {
        rv = PS_ERROR_OBJ_CREATE;
        goto error;
    }

    return rv;

error:
    log_err("could not create ps_regex_t. error: %d\n", rv);
    return rv;
}

ps_status_t
ps_regex_find(ps_regex_t* re,
              const char* content,
              unsigned int content_offset)
{
    int pcre_exec_ret = pcre_exec(re->regex_compiled,
                                  re->pcre_extra,
                                  content,
                                  strlen(content),
                                  content_offset,
                                  0,
                                  re->result_offset,
                                  MAX_REGEX_RESULT_OFFSET);

    if (pcre_exec_ret < 0)
    {
        re->error_code = pcre_exec_ret;
        re->found = FALSE;

        return PS_SUCCESS;
    }
    else
    {
        re->found = TRUE;
    }

    return PS_SUCCESS;
}

char*
ps_regex_error_code_to_str(ps_regex_t* re)
{
    switch(re->error_code)
    {
          case PCRE_ERROR_NOMATCH      : return "String did not match the pattern\n";
          case PCRE_ERROR_NULL         : return "Something was null\n";
          case PCRE_ERROR_BADOPTION    : return "A bad option was passed\n";
          case PCRE_ERROR_BADMAGIC     : return "Magic number bad (compiled re corrupt?)\n";
          case PCRE_ERROR_UNKNOWN_NODE : return "Something kooky in the compiled re\n";
          case PCRE_ERROR_NOMEMORY     : return "Ran out of memory\n";
          default                      : return "Unknown error\n";
    }
}

ps_status_t
ps_regex_free(ps_regex_t *re)
{
    pcre_free(re->regex_compiled);
    pcre_free(re->pcre_extra);

    re->regex_compiled = NULL;
    re->pcre_extra = NULL;

    return PS_SUCCESS;
}
