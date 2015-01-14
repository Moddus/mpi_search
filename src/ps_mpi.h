#ifndef PS_MPI_H
#define PS_MPI_H

#include <mpi.h>

#define PS_MPI_CHECK_GOTO_ERROR(rv) PS_CHECK_ERRV_AND_GOTO(rv, MPI_SUCCESS, error)

#endif
