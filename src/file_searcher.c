#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "regex.h"
#include "file_searcher.h"
#include "util.h"
#include "log.h"

#define BUFFER_SIZE ( 50000 )

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
                        char** result, size_t *result_len)
{
    ps_status_t rv = PS_SUCCESS;
    char *buffer = NULL;
    size_t buffer_offset = 0, buffer_fillsize = 0, bytes_read = 0;
    unsigned long read_limit = 0, total_read_count = 0, processed_bytes = 0, read_chunk_size = 0;

    log_debug("%s:begin", __func__);
    read_chunk_size = (searcher->task->file_read_chunk_size < searcher->task->size) ?
            searcher->task->file_read_chunk_size : searcher->task->size;
    PS_MALLOC(buffer, sizeof(char) * read_chunk_size);

    *result_len = 0;
    ps_file_searcher_task_debug(searcher);

    read_limit = searcher->task->size;
    log_debug("%s:read_limit:%lu read_chunk_size:%lu", __func__, read_limit, read_chunk_size);

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

    // move to next line break if not the first segment
    char c;
    if(searcher->task->offset > 0)
    {
        while ((c = getc(file)) != EOF) {
            if (c == '\n')
                break;
        }
    }
    log_debug("%s:before searching: total_read_count:%lu", __func__, total_read_count);

    //TODO: MAX_BYTES_TO_READ nur wenn kleiner als read_limit. Konfigurierbar machen
    while ( (bytes_read = fread(buffer + buffer_fillsize, sizeof(char),
            read_chunk_size - buffer_fillsize, file)) > 0)
    {
        char *search_start = NULL, *line_end = NULL;

        processed_bytes = 0;
        buffer_fillsize += bytes_read;
        search_start = buffer;
        log_debug("%s:buffer_offest:%lu bytes_read:%lu total_read_count:%lu, buffer_fillsize:%lu",
                  __func__, buffer_offset, bytes_read, total_read_count, buffer_fillsize);

        while ( (buffer_fillsize > 0) && (line_end = memchr(search_start, '\n', buffer_fillsize)))
        {
            ssize_t line_len = line_end - search_start;
            buffer_fillsize -= (line_len + 1);
            PS_CHECK_GOTO_ERROR(ps_regex_find(searcher->regex, buffer, line_len, (search_start-buffer)));
            search_start = line_end + 1;
            processed_bytes += line_len + 1;
        }

        total_read_count += processed_bytes;
        log_debug("%s:nothing more to read in buffer: bytes_read:%lu total_read_count:%lu buffer_fillsize:%lu"
                , __func__, bytes_read, total_read_count, buffer_fillsize);

        if( total_read_count >= read_limit){
            log_debug("%s:search-task done", __func__);
            break;
        }

        buffer_offset = processed_bytes;
        memmove(buffer, buffer + buffer_offset, buffer_fillsize);
    }

    log_debug("%s:end", __func__);
    return rv;

error:
    log_err("Error while searching: %d", rv);
    searcher->error = rv;
    PS_FREE(buffer);

    return rv;
    return PS_SUCCESS;}

ps_status_t
ps_searcher_task_create(ps_search_task_t **task,
                        unsigned long offset,
                        unsigned long size,
                        unsigned long chunk_size,
                        size_t path_len,
                        char* path)
{
    ps_status_t rv = PS_SUCCESS;

    log_debug("%s:begin", __func__);
    PS_MALLOC(*task, sizeof(char) * path_len + sizeof(ps_search_task_t));

    (*task)->offset = offset;
    (*task)->size = size;
    (*task)->file_read_chunk_size = chunk_size;
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

