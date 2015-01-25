#include <pcre.h>
#include <string.h>

#include "regexw.h"
#include "util.h"
#include "log.h"

ps_status_t
ps_regex_create(ps_regex_t **re, char *regex) {
    ps_status_t rv = PS_SUCCESS;

    log_debug("%s:begin", __func__);
    PS_MALLOC(*re, sizeof(ps_regex_t));
    (*re)->regex = regex;
    (*re)->error_offset = 0;
    (*re)->regex_compiled = pcre_compile((*re)->regex, 0,
            &(*re)->error,
            &(*re)->error_offset, NULL);
    (*re)->found = FALSE;

    if ((*re)->regex_compiled == NULL) {
        rv = PS_ERROR_OBJ_CREATE;
        goto error;
    }

    (*re)->pcre_extra = pcre_study((*re)->regex_compiled,
            0,
            &(*re)->error);

    if ((*re)->pcre_extra == NULL) {
        rv = PS_ERROR_OBJ_CREATE;
        goto error;
    }

    log_debug("%s:end", __func__);
    return rv;

    error:
    log_err("%s-error:could not create ps_regex_t. error: %d\n", __func__, rv);
    return rv;
}

ps_status_t
ps_regex_find(ps_regex_t *re,
        const char *content,
        size_t content_len,
        unsigned long content_offset) {
    int pcre_exec_ret = 0;

//    log_debug("%s:begin", __func__);
    pcre_exec_ret = pcre_exec(re->regex_compiled,
            re->pcre_extra,
            content,
            content_len,
            content_offset,
            0,
            re->result_offset,
            MAX_REGEX_RESULT_OFFSET);

    if (pcre_exec_ret < 0) {
        re->error_code = pcre_exec_ret;
        re->found = FALSE;
    }
    else {
        re->found = TRUE;
    }

//    log_debug("%s:end", __func__);
    return PS_SUCCESS;
}

char *
ps_regex_error_code_to_str(ps_regex_t *re) {
    switch (re->error_code) {
        case PCRE_ERROR_NOMATCH      :
            return "String did not match the pattern\n";
        case PCRE_ERROR_NULL         :
            return "Something was null\n";
        case PCRE_ERROR_BADOPTION    :
            return "A bad option was passed\n";
        case PCRE_ERROR_BADMAGIC     :
            return "Magic number bad (compiled re corrupt?)\n";
        case PCRE_ERROR_UNKNOWN_NODE :
            return "Something kooky in the compiled re\n";
        case PCRE_ERROR_NOMEMORY     :
            return "Ran out of memory\n";
        default                      :
            return "Unknown error\n";
    }
}

ps_status_t
ps_regex_free(ps_regex_t *re) {
    log_debug("%s:begin", __func__);
    pcre_free(re->regex_compiled);
    pcre_free(re->pcre_extra);

    re->regex_compiled = NULL;
    re->pcre_extra = NULL;

    log_debug("%s:end", __func__);
    return PS_SUCCESS;
}

