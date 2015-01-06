#ifndef FILE_SEARCHER_H
#define FILE_SEARCHER_H

#include <stdio.h>

typedef struct ps_file_seacher_t {
    unsigned int seek_to;
    int found;
    FILE *file;
} ps_file_seacher_t;

void
ps_file_searcher_create( ps_file_seacher_t** searcher,
                        unsigned int seek_to,
                        FILE* file );

#endif
