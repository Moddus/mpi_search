#ifndef MPI_FUNCTIONS_H
#define MPI_FUNCTIONS_H

#include <mpi.h>

#include "file_searcher.h"

#define MASTER 0

#define PS_MPI_TAG_PATH_LENGTH 0
#define PS_MPI_TAG_SEARCH_TASK 1

/**
 * Divides the path into peaces for searching. The master_task is a lot smaller,
 * so that it can focus on reducing the results.
 *
 * @path name of the file, that should be searched, has to be \n-terminated
 * @number_of_slave_procs number of slave processes
 * @slave_proc_numbers array of size number_of_slave_procs, containts the numbers of the slave procs
 * @comm MPI cummunicator
 * @master_task task for the master_process, has to be null
 */
int
distribute_path_and_search_range(char *path,
                                 unsigned int number_of_procs,
                                 int *slave_proc_numbers,
                                 unsigned long chunk_size,
                                 MPI_Comm comm,
                                 ps_search_task_t **master_task);


int
recv_task(ps_search_task_t **task, int own_rank, int master, MPI_Comm comm);

#endif

