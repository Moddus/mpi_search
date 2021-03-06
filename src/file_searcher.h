#ifndef FILE_SEARCHER_H
#define FILE_SEARCHER_H

#include "util.h"
#include "regexw.h"

#define DEFAULT_CHUNK_SIZE ( 1024 * 1024 * 10 )

typedef struct ps_search_task{
    /*Defines the area to search in*/
    unsigned long offset, size;
    /*Bytes to read per chunk*/
    unsigned long file_read_chunk_size;
    /*Length of path*/
    size_t path_len;
    /*column number vor CSV*/
    int col_num;
    /*Filename of the file, that should be searched*/
    char path[];
} ps_search_task_t;

typedef struct ps_searcher_t
{
    ps_regex_t* regex;
    ps_search_task_t* task;
    ps_bool_t error;

} ps_searcher_t;

ps_status_t
ps_file_searcher_create(ps_searcher_t** seacher,
                        char* search,
                        ps_search_task_t* task);

ps_status_t
ps_file_searcher_free(ps_searcher_t** searcher);

ps_status_t
ps_file_searcher_search(ps_searcher_t* seacher,
                        char** result,
                        size_t *result_len);


ps_status_t
ps_searcher_task_create(ps_search_task_t **task,
                             unsigned long offset,
                             unsigned long size,
                             unsigned long chunk_size,
                             int search_col,
                             size_t path_len,
                             char* path);

ps_status_t
ps_searcher_task_free(ps_search_task_t** task);

#endif
