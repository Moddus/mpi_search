#include <mpi.h>
#include <string.h>

#include "mpi_functions.h"
#include "util.h"
#include "log.h"
#include "file_util.h"

int
malloc_and_set_ps_search_task(ps_search_task_t **task, unsigned long start, unsigned long offset,
                              int filename_size, char* filename)
{
    ps_status_t rv = PS_SUCCESS;
    size_t search_task_mem_size = 0;

    log_debug("malloc_and_set_ps_search_task:begin");
    search_task_mem_size = sizeof(char) * filename_size + sizeof(ps_search_task_t);
    PS_MALLOC(*task, search_task_mem_size);

    (*task)->start = start;
    (*task)->offset = offset;
    (*task)->filename_size = filename_size;

    PS_COMPARE_GOTO_ERROR(
        strlcpy((*task)->filename, filename, search_task_mem_size + 1), /*+1 for \n*/
        search_task_mem_size,
        PS_ERROR_COPY);
    log_debug("malloc_and_set_ps_search_task:end");
    return rv;
error:
    PS_FREE(*task);
    log_err("malloc_and_set_ps_search_task:error");
    return rv;
}

int
distribute_filename_and_search_range(char *filename, int number_of_slave_procs,
                                     int *slave_proc_numbers,
                                     MPI_Comm comm,
                                     ps_search_task_t **master_task)
{
    ps_status_t rv = PS_SUCCESS;
    unsigned long total_filesize = 0, slave_search_range_size = 0, master_search_range_size = 0, start = 0;
    ps_search_task_t **slave_tasks = NULL;
    int filename_len = strlen(filename);
    int i = 0;
    size_t search_task_mem_size = 0;
    MPI_Request *requests = NULL;

    log_debug("distribute_filename_and_search_range:begin. number_of_slave_procs:%d", number_of_slave_procs);

    if (filename == NULL || number_of_slave_procs < 1 || *master_task != NULL)
    {
        log_err("wrong arguments in distribute_filename_and_search_range: \
                 filename=%s, number_of_slave_procs:%d, master_task:%p",
                 filename, number_of_slave_procs, *master_task);
        rv = PS_ERROR_WRONG_ARGUMENTS;
        goto error;
    }

    search_task_mem_size = sizeof(char) * filename_len + sizeof(ps_search_task_t);

    PS_CHECK_GOTO_ERROR(get_filesize(filename, &total_filesize));
    log_debug("total_filesize:%lu", total_filesize);

    slave_search_range_size = total_filesize / number_of_slave_procs;
    /*master searches only in a small range and focuses on reducing the results*/
    master_search_range_size = total_filesize % number_of_slave_procs;
    log_debug("slave_search_range_size : %lu, master_search_range_size: %lu",
              slave_search_range_size, master_search_range_size);

    /*Set search_task for master*/
    PS_CHECK_GOTO_ERROR( malloc_and_set_ps_search_task(master_task, 0, master_search_range_size - 1,
                         filename_len, filename));

    PS_MALLOC(slave_tasks, sizeof(ps_search_task_t*) * number_of_slave_procs);

    /*Need two requests per slave*/
    PS_MALLOC(requests, 2 * sizeof(MPI_Request) * number_of_slave_procs);
    /*Tell the slaves in which ranges and which files to search */
    for (i = 0, start = master_search_range_size ; i < number_of_slave_procs - 1; i++)
    {
        PS_CHECK_GOTO_ERROR( malloc_and_set_ps_search_task(
                                 &slave_tasks[i],
                                 start + i * slave_search_range_size,
                                 slave_search_range_size,
                                 filename_len,
                                 filename));

        MPI_Isend(&filename_len, 1, MPI_INT, slave_proc_numbers[i],
                  PS_MPI_TAG_FILENAME_LENGTH, comm, &requests[2 * i]);
        MPI_Isend(&slave_tasks[i], search_task_mem_size, MPI_BYTE, slave_proc_numbers[i],
                  PS_MPI_TAG_SEARCH_TASK, comm, &requests[2 * i + 1]);
    }

    PS_CHECK_GOTO_ERROR(MPI_Waitall(number_of_slave_procs, requests, MPI_STATUSES_IGNORE));

    PS_FREE(slave_tasks);
    PS_FREE(requests);
    log_debug("distribute_filename_and_search_range:end");
    return rv;

error:
    PS_FREE(*master_task);
    PS_FREE(slave_tasks);
    PS_FREE(requests);
    return rv;
}

