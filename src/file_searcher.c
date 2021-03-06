#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "regex.h"
#include "file_searcher.h"
#include "util.h"
#include "log.h"
#include "csv.h"

#define START_RESULT_BUFFER_SIZE ( 50000 )

#ifdef TIME_MEASUREMENT
#include "timeutil.h"

extern float process_search_time;
extern float process_file_io_time;
#endif

static void
ps_file_searcher_task_debug(ps_searcher_t *searcher)
{
    log_debug("%s:begin", __func__);

    if (searcher->task)
    {
        log_debug("%s:Task: offset:%lu size:%lu file_read_chunk_size:%lu path_len:%lu path:%s",
                  __func__,
                  searcher->task->offset,
                  searcher->task->size,
                  searcher->task->file_read_chunk_size,
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
ps_file_searcher_search(ps_searcher_t* searcher,
                        char** result,
                        size_t *result_len)
{
    ps_status_t rv = PS_SUCCESS;
    char *buffer = NULL, *result_c = NULL;
    size_t buffer_offset = 0, buffer_fillsize = 0, bytes_read = 0, result_buffer_size = START_RESULT_BUFFER_SIZE;
    unsigned long read_limit = 0, total_read_count = 0, processed_bytes = 0, read_chunk_size = 0;

    log_debug("%s:begin", __func__);
    read_chunk_size = (searcher->task->file_read_chunk_size < searcher->task->size) ?
            searcher->task->file_read_chunk_size : searcher->task->size;
    PS_MALLOC(buffer, sizeof(char) * read_chunk_size);

    *result_len = 0;
    ps_file_searcher_task_debug(searcher);

    read_limit = searcher->task->size;
    log_debug("%s:read_limit:%lu read_chunk_size:%lu", __func__, read_limit, read_chunk_size);

    PS_MALLOC(*result, sizeof(char) * result_buffer_size);
    result_c = *result;

#ifdef TIME_MEASUREMENT
    struct timeval last_timestemp;
    gettimeofday(&last_timestemp, NULL);
#endif

    // open file
    FILE *file = fopen(searcher->task->path, "r");
    PS_CHECK_PTR_NULL(file, PS_ERROR_FAILED_TO_OPEN_FILE);

    // move to next line break
    char c;
    if(searcher->task->offset > 0 )
    {
        // seek to position. -1 to check if last_sign was a EOF.
        // Then the current offset is the beginning of a line
        fseek(file, searcher->task->offset - 1, SEEK_CUR);
        while ((c = getc(file)) != EOF) {
            if (c == '\n')
                break;
        }
    }

    log_debug("%s:seeked to start-position: %lu", __func__, searcher->task->offset);
    log_debug("%s:before searching: total_read_count:%lu", __func__, total_read_count);

    while ( (bytes_read = fread(buffer + buffer_fillsize, sizeof(char),
            read_chunk_size - buffer_fillsize, file)) > 0)
    {
        char *search_start = NULL, *line_end = NULL;
        int line_end_found = FALSE;

#ifdef TIME_MEASUREMENT
        update_timestamp_and_total_seconds(&last_timestemp, &process_file_io_time);
#endif

        processed_bytes = 0;
        buffer_fillsize += bytes_read;
        search_start = buffer;
//        log_debug("%s:buffer_offest:%lu bytes_read:%lu total_read_count:%lu, buffer_fillsize:%lu",
//                  __func__, buffer_offset, bytes_read, total_read_count, buffer_fillsize);

        while ( (total_read_count < read_limit) &&
                (buffer_fillsize > 0) && (line_end = memchr(search_start, '\n', buffer_fillsize)))
        {
            line_end_found = TRUE;
            size_t line_len = line_end - search_start + 1;
            buffer_fillsize -= line_len;
            char *val = NULL;
            size_t val_len;

            PS_CHECK_GOTO_ERROR(ps_csv_get_column(search_start, line_len, &val, &val_len, searcher->task->col_num));
            if(NULL != val)
            {
                PS_CHECK_GOTO_ERROR(ps_regex_find(searcher->regex, val, val_len, 0));

                if(searcher->regex->found)
                {
                    while((result_buffer_size - *result_len) < line_len)
                    {
                        result_buffer_size += START_RESULT_BUFFER_SIZE;
                        PS_REALLOC(*result , sizeof(char) * result_buffer_size);
                        result_c = *result + *result_len;
                    }
                    memcpy(result_c, search_start, line_len);
                    *result_len += line_len;
                    result_c += line_len;
                }

                if(val != search_start)
                {
                    PS_FREE(val);
                }
            }
            search_start = line_end + 1;
            processed_bytes += line_len;
            total_read_count += line_len;
        }

        if(total_read_count >= read_limit){
            log_debug("%s:search-task done", __func__);
            break;
        }

        /*Increase buffer size if too small for one line -> it does not find a newline*/
        if(!line_end_found)
        {
            read_chunk_size *= 2;
            log_debug("%s:buffer_size increased to %lu", __func__, read_chunk_size);
            PS_REALLOC(buffer, sizeof(char) * read_chunk_size);
        }

        buffer_offset = processed_bytes;


        memmove(buffer, buffer + buffer_offset, buffer_fillsize);

#ifdef TIME_MEASUREMENT
        update_timestamp_and_total_seconds(&last_timestemp, &process_search_time);
#endif
    }

    PS_CLOSE_FILE(file);
    log_debug("%s:end searcher_task_offset:%lu read_limit:%lu total_read_bytes:%lu", __func__,
            searcher->task->offset, read_limit, total_read_count);
    return rv;

error:
    log_err("%s:Error while searching: %d", __func__, rv);
    PS_CLOSE_FILE(file);
    searcher->error = rv;
    PS_FREE(buffer);

    return rv;
}

ps_status_t
ps_searcher_task_create(ps_search_task_t **task,
                        unsigned long offset,
                        unsigned long size,
                        unsigned long chunk_size,
                        int search_col,
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
    (*task)->col_num = search_col;

    strncpy((*task)->path, path, path_len);

    log_debug("%s:end path:%s", __func__, (*task)->path);

    return rv;

error:
    log_debug("%s:error", __func__);
    PS_FREE(*task);

    return rv;
}

ps_status_t
ps_file_searcher_free(ps_searcher_t** searcher)
{
    log_debug("%s:begin", __func__);
    ps_regex_free((*searcher)->regex);
    ps_searcher_task_free(&(*searcher)->task);
    PS_FREE(*searcher);

    log_debug("%s:end", __func__);
    return PS_SUCCESS;
}

ps_status_t
ps_searcher_task_free(ps_search_task_t** task)
{
    PS_FREE(*task);
    return PS_SUCCESS;
}

