#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#define FALSE ( 0 )
#define TRUE  ( 1 )

/*
 * Typedefs
 */

typedef int ps_status_t;

/*
 * Error codes
 */

#define PS_SUCCESS          ( 0 )
#define PS_ALLOCATION_ERROR ( 1 )
#define PS_WRONG_ARGUMENTS  ( 2 )
#define PS_FAILED_TO_OPEN_FILE ( 3 )

/*
 * Error handling
 */

#define PS_CHECK_NEG_RET_AND_GOTO(rv, go)   \
    if(rv < 0) {                            \
        goto go;                            \
    }

#define  PS_CHECK_NEG_RET_AND_GOTO_ERROR(rv) PS_CHECK_NEG_RET_AND_GOTO(rv, error)

#define PS_CHECK_AND_GOTO(rv, go)         \
    if(rv != PS_SUCCESS) {                \
        goto go;                          \
    }

#define PS_CHECK_GOTO_ERROR(rv) PS_CHECK_AND_GOTO(rv, error)

#define PS_CHECK_PTR_AND_GOTO(ptr, rv)      \
    if (ptr == NULL) {                    \
        rv = PS_ALLOCATION_ERROR;         \
        goto error;                       \
    }

#define PS_CHECK_PTR_NULL(PTR, RV){                 \
    if ( !PTR )                                     \
    {                                               \
        rv = RV;                                    \
        if ( errno )                                \
        {                                           \
            log_err( strerror( errno ) );           \
        }                                           \
        goto error;                                 \
    }                                               \
}


#define PS_MALLOC(PTR, SIZE) {            \
    PTR = malloc(SIZE);                   \
    PS_CHECK_PTR_AND_GOTO(PTR, rv)          \
}

#define PS_FREE(PTR){                       \
    if(ptr)                                 \
    {                                       \
        free(ptr);                          \
        ptr = NULL;                         \
    }                                       \
}

#define PS_CLOSE_FILE(FILE){                \
    if(FILE)                                \
    {                                       \
        fclose(FILE);                       \
        FILE = NULL;                        \
    }                                       \
}


/*
 * Logging
 */

FILE *out_fd;

#endif
