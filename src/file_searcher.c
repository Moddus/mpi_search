#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regex.h"
#include "file_searcher.h"
#include "util.h"
#include "log.h"

#define BUFFER_SIZE ( 1000 )

static void
ps_file_searcher_task_debug(ps_searcher_t *searcher)
{
    log_debug("%s:begin", __func__);

    if (searcher->task)
    {
        log_debug("Task: offset:%lu size:%lu path_len:%lu path:%s",
                  searcher->task->offset,
                  searcher->task->size,
                  searcher->task->path_len,
                  searcher->task->path);
    }

    log_debug("%s:end", __func__);
}

ps_status_t
ps_file_searcher_create(ps_searcher_t** searcher,
                        char* search,
                        ps_search_task_t* task)
{
    ps_status_t rv = PS_SUCCESS;
    ps_regex_t* regex;

    log_debug("%s:begin", __func__);
    PS_MALLOC(*searcher, sizeof(ps_searcher_t));
    PS_CHECK_GOTO_ERROR(ps_regex_create(&regex, search));
    log_debug("%s:ps_regex_created", __func__);

    (*searcher)->regex = regex;
    (*searcher)->task = task;

    log_debug("%s:end", __func__);
    return rv;

error:
    log_err("ps_file_searcher_t creation faild: %d", PS_ERROR_ALLOCATION);

    return rv;
}

ps_status_t
ps_file_searcher_free(ps_searcher_t** searcher)
{
    log_debug("%s:begin", __func__);
    ps_regex_free((*searcher)->regex);
    ps_searcher_task_free((*searcher)->task);
    PS_FREE(*searcher);

    log_debug("%s:end", __func__);
    return PS_SUCCESS;
}

ps_status_t
ps_file_searcher_search(ps_searcher_t* searcher,
                        char** result)
{
    ps_status_t rv = PS_SUCCESS;
    unsigned long read_limit = 0;
    unsigned long read_count = 0;
    // get enougth space to read
    unsigned int buffer_len = BUFFER_SIZE;
    char buffer[buffer_len];
    unsigned int result_len = 0;
    unsigned int result_free_space = BUFFER_SIZE;
    log_debug("%s:begin", __func__);

    ps_file_searcher_task_debug(searcher);

    read_limit = searcher->task->size;
    log_debug("%s:read_limit:%lu", __func__, read_limit);

    PS_MALLOC(*result, sizeof(char) * BUFFER_SIZE);

    // open file
    FILE *file = fopen(searcher->task->path, "r");
    if ( NULL == file )
    {
        log_err("%s: could not open file", __func__);
        return PS_ERROR_FAILED_TO_OPEN_FILE;
    }
    // seek to position
    fseek(file, searcher->task->offset, SEEK_CUR);
    log_debug("%s:seeked to start-position: %lu", __func__, searcher->task->offset);

    // move to next line break
    char c;
    while ( ( c = getc(file) ) != EOF )
    {
        read_count++;
        if (read_count > read_limit)
        {
            return PS_ERROR_CHUNCK_TO_SHORT;
        }

        if (c == '\n')
            break;
    }
    log_debug("%s:current offset:%lu", __func__, fseek(file, 0, SEEK_CUR));

    while ( fgets(buffer, buffer_len, file) != NULL )
    {
        unsigned int line_len = strlen(buffer) + 1;
        log_debug("%s:line_len:%u", __func__, line_len);
        read_count += line_len;
        if (read_count > read_limit)
        {
            log_debug("%s:searching done at:%lu", __func__, fseek(file, 0, SEEK_CUR));
            break;
        }

        PS_CHECK_GOTO_ERROR(ps_regex_find(searcher->regex, buffer, 0));
        if (searcher->regex->found == TRUE)
        {
            log_debug("%s:found someting", __func__);
            if (result_free_space < line_len)
            {
                log_debug("%s: buffer gets realloced", __func__);
                PS_REALLOC(*result, sizeof(char) * (result_len + BUFFER_SIZE));
            }

            strncpy((*result) + result_len, buffer, line_len);
            log_debug("%s: line copied to result", __func__);
            result_free_space -= line_len;
            result_len += line_len;
            log_debug("%s: new result_len:%u result_free_space:%u", __func__, result_len, result_free_space);
        }
    }

    log_debug("%s:end", __func__);
    return rv;

error:
    log_err("Error while searching: %d", rv);
    searcher->error = rv;

    return rv;
}

ps_status_t
ps_searcher_task_create(ps_search_task_t **task,
                        unsigned long offset,
                        unsigned long size,
                        unsigned int path_len,
                        char* path)
{
    ps_status_t rv = PS_SUCCESS;

    log_debug("%s:begin", __func__);
    PS_MALLOC(*task, sizeof(char) * path_len + sizeof(ps_search_task_t));

    (*task)->offset = offset;
    (*task)->size = size;
    (*task)->path_len = path_len;

    strncpy((*task)->path, path, path_len + 1);

    log_debug("%s:end", __func__);

    return rv;

error:
    log_debug("%s:error", __func__);
    PS_FREE(*task);

    return rv;
}

ps_status_t
ps_searcher_task_free(ps_search_task_t* task)
{
    PS_FREE(task);

    return PS_SUCCESS;
}

