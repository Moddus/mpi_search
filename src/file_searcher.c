#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regex.h"
#include "file_searcher.h"
#include "util.h"
#include "log.h"

#define BUFFER_SIZE ( 1000 )

ps_status_t
ps_file_searcher_create(ps_searcher_t** searcher,
                        char* search,
                        ps_search_task_t* task)
{
    ps_status_t rv = PS_SUCCESS;
    ps_regex_t* regex;

    PS_MALLOC(*searcher, sizeof(ps_searcher_t));
    PS_CHECK_GOTO_ERROR(ps_regex_create(&regex, search));

    (*searcher)->regex = regex;
    (*searcher)->task = task;

    return rv;

error:
    log_err("ps_file_searcher_t creation faild: %d", PS_ERROR_ALLOCATION);

    return rv;
}

ps_status_t
ps_file_searcher_free(ps_searcher_t** searcher)
{
    ps_regex_free((*searcher)->regex);
    ps_searcher_task_free((*searcher)->task);
    PS_FREE(*searcher);

    return PS_SUCCESS;
}

ps_status_t
ps_file_searcher_search(ps_searcher_t* searcher,
                        char** result)
{
    ps_status_t rv = PS_SUCCESS;
    int read_limit = searcher->task->size;
    int read_count = 0;
    // get enougth space to read
    unsigned int buffer_len = searcher->task->size * 2;
    char buffer[buffer_len];
    unsigned int result_len = 0;
    unsigned int result_free_space = BUFFER_SIZE;
    log_debug("%s:begin", __func__);

    PS_MALLOC(*result, sizeof(char) * BUFFER_SIZE);

    log_debug("ps_file_searcher_search:begin");

    // open file
    FILE *file = fopen(searcher->task->path, "r");
    if ( NULL == file )
    {
        return PS_ERROR_FAILED_TO_OPEN_FILE;
    }
    // seek to osition
    fseek(file, searcher->task->offset, SEEK_CUR);

    // move to next line break
    char c;
    while( ( c = getc(file) ) != EOF )
    {
        read_count++;
        if(read_count > read_limit)
        {
            return PS_ERROR_CHUNCK_TO_SHORT;
        }

        if(c == '\n')
            break;
    }

    while( fgets(buffer, buffer_len, file) != NULL )
    {
        int line_len = strlen(buffer) + 1;
        read_count += line_len;
        if(read_count > read_limit)
            break;

        log_debug("read: %s block: %d count: %d\n", buffer, strlen(buffer), read_count);
        PS_CHECK_GOTO_ERROR(ps_regex_find(searcher->regex, buffer, 0));
        if(searcher->regex->found == TRUE)
        {
            if(result_free_space < line_len)
            {
                PS_REALLOC(*result, sizeof(char) * BUFFER_SIZE);
            }

            strncpy((*result) + result_len, buffer, line_len);
            result_free_space -= line_len;
            result_len += line_len;
        }
    }

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
                        unsigned long path_len,
                        char* path)
{
    ps_status_t rv = PS_SUCCESS;

    log_debug("%s:begin", __func__);
    PS_MALLOC(*task, sizeof(char) * path_len + sizeof(ps_search_task_t));

    (*task)->offset = offset;
    (*task)->size = size;
    (*task)->path_len = path_len;

    PS_COMP(
        strlcpy((*task)->path, path, path_len + 1), /*+1 for \n*/
        path_len,
        PS_ERROR_COPY);
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

