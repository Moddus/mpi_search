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

/*
 * Error handling
 */

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

#define PS_MALLOC(PTR, SIZE) {            \
    PTR = malloc(SIZE);                   \
    PS_CHECK_PTR_AND_GOTO(PTR, rv)          \
}

/*
 * Logging
 */

FILE *out_fd;

#endif
