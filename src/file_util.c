#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "file_util.h"
#include "util.h"
#include "log.h"

int
get_filesize(char *filename, long *filesize)
{
    ps_status_t rv = PS_SUCCESS; 
    FILE *searchfile = NULL;

    /*----Calc the total filesize of the file, that should be analyzed---*/
    PS_CHECK_PTR_NULL((searchfile = fopen(filename, "r")), PS_FAILED_TO_OPEN_FILE);
    PS_CHECK_GOTO_ERROR(fseek(searchfile, 0L, SEEK_END));
    PS_CHECK_NEG_RET_AND_GOTO_ERROR((*filesize = ftell(searchfile)));
    log_debug("Total filesize: %lu", *filesize);
    PS_CLOSE_FILE(searchfile);

    return rv;

error:
    PS_CLOSE_FILE(searchfile);
    return rv;
}
