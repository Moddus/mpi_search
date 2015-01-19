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

#define PS_SUCCESS          ( 101 )
#define PS_ERROR_COPY       ( 102 )
#define PS_ERROR_ALLOCATION ( 103 )
#define PS_ERROR_WRONG_ARGUMENTS  ( 104 )
#define PS_ERROR_OBJ_CREATE ( 105 )

// I/O
#define PS_ERROR_FAILED_TO_OPEN_FILE ( 201 )
#define PS_ERROR_FAILED_TO_SEEK ( 202 )
#define PS_ERROR_FAILED_TO_CALC_SIZE ( 203 )

// Regex
#define PS_ERROR_REGEX_FOUND_IS_FALSE ( 301 )

// MPI
#define PS_MPI_COMMON_ERROR ( 401 )

/*
 * Error handling
 */
#define PS_COMP_VAL_AND_GOTO_WITH_RV(R, OP, VAL, GO, RV)         \
    if(R OP VAL) {                \
        rv = RV;                                                    \
        log_err("file : %s line: %d func: %s rv:%d", __FILE__, __LINE__, __func__, rv);\
        goto GO;                          \
    }

#define PS_CHECK_VAL_GO_ERR(R, VAL, RV) PS_COMP_VAL_AND_GOTO_WITH_RV(R, !=, VAL, error, RV)

#define PS_CHECK_NEG_VAL_GO_ERR(R, RV) PS_COMP_VAL_AND_GOTO_WITH_RV(R, <, 0, error, RV)

#define PS_COMP(VAL1, VAL2, RV) PS_COMP_VAL_AND_GOTO_WITH_RV(VAL1, !=, VAL2, error, RV)

#define PS_CHECK_VAL_AND_GOTO(RV, VAL, GO)         \
    if(RV != VAL) {                \
        log_err("file : %s line: %d func: %s", __FILE__, __LINE__, __func__);\
        goto GO;                          \
    }

#define PS_CHECK_AND_GOTO(RV, GO) PS_CHECK_VAL_AND_GOTO(RV, PS_SUCCESS, GO)

#define PS_CHECK_GOTO_ERROR(RV) PS_CHECK_AND_GOTO(RV, error)

#define PS_CHECK_PTR_AND_GOTO_ERROR(PTR)  \
    if (PTR == NULL) {                    \
        rv = PS_ERROR_ALLOCATION;         \
        goto error;                       \
    }

#define PS_MALLOC(PTR, SIZE) {            \
    PTR = malloc(SIZE);                   \
    PS_CHECK_PTR_AND_GOTO_ERROR(PTR)      \
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

/*
 * Logging
 */

FILE *out_fd;

#endif
