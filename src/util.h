#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define FALSE ( 0 )
#define TRUE  ( 1 )

/*
 * Typedefs
 */

typedef int ps_status_t;

/*
 * Error codes
 */

#define PS_SUCCESS          ( 101 )
#define PS_ERROR_COPY       ( 102 )
#define PS_ERROR_ALLOCATION ( 103 )
#define PS_ERROR_WRONG_ARGUMENTS  ( 104 )
#define PS_ERROR_OBJ_CREATE ( 105 )

// I/O
#define PS_ERROR_FAILED_TO_OPEN_FILE ( 201 )

// Regex
#define PS_ERROR_REGEX_FOUND_IS_FALSE ( 301 )

/*
 * Error handling
 */

#define PS_CHECK_NEG_RET_AND_GOTO(rv, go)   \
    if(rv < 0) {                            \
        goto go;                            \
    }

#define  PS_CHECK_NEG_RET_AND_GOTO_ERROR(rv) PS_CHECK_NEG_RET_AND_GOTO(rv, error)

#define PS_CHECK_ERRV_AND_GOTO(rv, errv, go)         \
    if(rv != errv) {                \
        log_err("file : %s line: %d func: %s rv: %d", __FILE__, __LINE__, __func__, rv);\
        goto go;                          \
    }

#define PS_CHECK_AND_GOTO(rv, go) PS_CHECK_ERRV_AND_GOTO(rv, PS_SUCCESS, go)

#define PS_CHECK_ERRV_GOTO_ERROR(rv, errv) PS_CHECK_ERRV_AND_GOTO(rv, errv, error)

#define PS_CHECK_GOTO_ERROR(rv) PS_CHECK_AND_GOTO(rv, error)

#define PS_MPI_CHECK_GOTO_ERROR(rv) PS_CHECK_ERRV_AND_GOTO(rv, MPI_SUCCESS, error)

#define PS_CHECK_PTR_AND_GOTO(ptr, rv)      \
    if (ptr == NULL) {                    \
        rv = PS_ERROR_ALLOCATION;         \
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
    if(PTR)                                 \
    {                                       \
        free(PTR);                          \
        PTR = NULL;                         \
    }                                       \
}

#define PS_CLOSE_FILE(FILE){                \
    if(FILE)                                \
    {                                       \
        fclose(FILE);                       \
        FILE = NULL;                        \
    }                                       \
}

#define PS_COMPARE_GOTO(VAL1, VAL2, RV, GO){    \
    if(VAL1 != VAL2){                               \
        rv = RV;                                    \
        goto GO;                                    \
    }                                               \
}

#define PS_COMPARE_GOTO_ERROR(VAL1, VAL2, RV) PS_COMPARE_GOTO(VAL1, VAL2, RV, error)

/*
 * Logging
 */

FILE *out_fd;

#endif
