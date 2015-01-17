#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "file_util.h"
#include "util.h"
#include "log.h"

int
get_filesize(char *filename, unsigned long *filesize)
{
    ps_status_t rv = PS_SUCCESS; 
    FILE *searchfile = NULL;

    log_debug("get_filesize:begin");

    /*----Calc the total filesize of the file, that should be analyzed---*/
    PS_CHECK_PTR_NULL((searchfile = fopen(filename, "r")), PS_ERROR_FAILED_TO_OPEN_FILE);
    PS_CHECK_VAL_GO_ERR(fseek(searchfile, 0L, SEEK_END), 0, PS_ERROR_FAILED_TO_SEEK);
    PS_CHECK_NEG_VAL_GO_ERR((*filesize = ftell(searchfile)), PS_ERROR_FAILED_TO_CALC_SIZE);
    log_debug("Total filesize: %lu", *filesize);
    PS_CLOSE_FILE(searchfile);

    log_debug("get_filesize:end");

    return rv;

error:
    PS_CLOSE_FILE(searchfile);
    log_debug("get_filesize:error");

    return rv;
}

