#include <stdio.h>
#include <search.h>
#include <stdlib.h>

#include "file_searcher.h"
#include "util.h"

void
ps_file_searcher_create( ps_file_seacher_t** searcher,
                        unsigned int seek_to,
                        FILE* file )
{
    //*searcher = ag_malloc(sizeof(ps_file_seacher_t));
    //(*searcher)->seek_to = seek_to;
    //(*searcher)->found = -1;
    //(*searcher)->file = file;
}

void
ps_file_searcher_free( ps_file_seacher_t* searcher )
{
   //free(searcher->file);
   //free(searcher);
}

void
ps_file_searcher_search(ps_file_seacher_t* seacher)
{
    // TODO silver searcher
}