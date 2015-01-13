#ifndef MPI_FUNCTIONS_H
#define MPI_FUNCTIONS_H 

#include <mpi.h>

#define MASTER 0

#define PS_MPI_TAG_FILENAME_LENGTH 0
#define PS_MPI_TAG_SEARCH_TASK 1

typedef struct ps_search_task{
    /*Gibt Bereich an, in dem gesucht werden soll, in Byte*/
    unsigned long start, offset;
    /*Laenge des Dateinamens*/
    unsigned int filename_len;
    /*Datei, die durchsucht werden soll*/
    char filename[];
} ps_search_task_t;

int
malloc_and_set_ps_search_task(ps_search_task_t **task, unsigned long start, unsigned long offset,
                              unsigned long filename_size, char* filename);

/**
 * Divides the filename into peaces for searching. The master_task is a lot smaller,
 * so that it can focus on reducing the results.
 *
 * @filename name of the file, that should be searched, has to be \n-terminated
 * @number_of_slave_procs number of slave processes
 * @slave_proc_numbers array of size number_of_slave_procs, containts the numbers of the slave procs
 * @comm MPI cummunicator
 * @master_task task for the master_process, has to be null
 */
int
distribute_filename_and_search_range(char *filename, int number_of_slave_procs,
                                     int *slave_proc_numbers,
                                     MPI_Comm comm,
                                     ps_search_task_t **master_task);


int
recv_task(ps_search_task_t **task, int own_rank, int master, MPI_Comm comm);

#endif

