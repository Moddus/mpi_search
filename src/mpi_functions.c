#include <mpi.h>
#include <string.h>

#include "ps_mpi.h"
#include "mpi_functions.h"
#include "util.h"
#include "log.h"
#include "file_util.h"

int
malloc_and_set_ps_search_task(ps_search_task_t **task, unsigned long offset, unsigned long size,
                              unsigned long filename_len, char* filename)
{
    ps_status_t rv = PS_SUCCESS;
    size_t search_task_mem_size = 0;

    log_debug("malloc_and_set_ps_search_task:begin");
    search_task_mem_size = sizeof(char) * filename_len + sizeof(ps_search_task_t);
    PS_MALLOC(*task, search_task_mem_size);

    (*task)->offset = offset;
    (*task)->size= size;
    (*task)->filename_len = filename_len;

    PS_COMP(
        strlcpy((*task)->filename, filename, filename_len + 1), /*+1 for \n*/
        filename_len,
        PS_ERROR_COPY);
    log_debug("malloc_and_set_ps_search_task:end");
    return rv;
error:
    PS_FREE(*task);
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

    PS_CALL(get_filesize(filename, &total_filesize));

    slave_search_range_size = total_filesize / number_of_slave_procs;
    /*master searches only in a small range and focuses on reducing the results*/
    master_search_range_size = total_filesize % number_of_slave_procs;
    log_debug("slave_search_range_size : %lu, master_search_range_size: %lu",
              slave_search_range_size, master_search_range_size);

    /*Set search_task for master*/
    PS_CALL( malloc_and_set_ps_search_task(master_task, 0, master_search_range_size - 1,
                                           filename_len, filename));

    PS_MALLOC(slave_tasks, sizeof(ps_search_task_t*) * number_of_slave_procs);

    /*Need two requests per slave*/
    PS_MALLOC(requests, 2 * sizeof(MPI_Request) * number_of_slave_procs);
    /*Tell the slaves in which ranges and which files to search */
    for (i = 0, start = master_search_range_size ; i < number_of_slave_procs; i++)
    {
        PS_CHECK_GOTO_ERROR( malloc_and_set_ps_search_task(
                                 &slave_tasks[i],
                                 start + i * slave_search_range_size,
                                 slave_search_range_size,
                                 filename_len,
                                 filename));

        log_debug("Process %d: offset:%lu, size:%lu, filename:%s, search_task_mem_size:%d",
                  i, slave_tasks[i]->offset, slave_tasks[i]->size, slave_tasks[i]->filename, search_task_mem_size);
        MPI_Isend(&filename_len, 1, MPI_INT, slave_proc_numbers[i],
                  PS_MPI_TAG_FILENAME_LENGTH, comm, &requests[2 * i]);
        MPI_Isend(slave_tasks[i], search_task_mem_size, MPI_BYTE, slave_proc_numbers[i],
                  PS_MPI_TAG_SEARCH_TASK, comm, &requests[2 * i + 1]);
    }

    PS_MPI_CHECK_ERR(MPI_Waitall(number_of_slave_procs, requests, MPI_STATUSES_IGNORE));
    for (i = 0; i < number_of_slave_procs; i++)
    {
        PS_FREE(slave_tasks[i]);
    }

    PS_FREE(slave_tasks);
    PS_FREE(requests);
    log_debug("distribute_filename_and_search_range:end");
    return rv;

error:
    PS_FREE(*master_task);
    if (slave_tasks)
    {
        for (i = 0; i < number_of_slave_procs; i++)
        {
            PS_FREE(slave_tasks[i]);
        }
    }

    PS_FREE(slave_tasks);
    PS_FREE(requests);
    return rv;
}

int
recv_task(ps_search_task_t **task, int own_rank, int master, MPI_Comm comm)
{
    ps_status_t rv = PS_SUCCESS;
    MPI_Status status;
    int filename_length = -1;
    size_t tasklen = 0;

    PS_MPI_CHECK_ERR(MPI_Recv(&filename_length, 1, MPI_INT, master, PS_MPI_TAG_FILENAME_LENGTH, comm, &status));

    tasklen = sizeof(char) * filename_length + sizeof(ps_search_task_t);
    log_debug("Process %d: filename_length:%d ,tasklen:%d", own_rank, filename_length, tasklen);
    PS_MALLOC(task, tasklen);
    PS_MPI_CHECK_ERR(MPI_Recv(task, tasklen, MPI_BYTE, master, PS_MPI_TAG_SEARCH_TASK, comm, &status));

    return rv;
error:
    PS_FREE(task);
    return rv;
}

