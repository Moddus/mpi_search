#include <mpi.h>
#include <string.h>

#include "ps_mpi.h"
#include "mpi_functions.h"
#include "util.h"
#include "log.h"
#include "file_util.h"

int
distribute_path_and_search_range(char *path,
                                 int number_of_procs,
                                 int *slave_proc_numbers,
                                 MPI_Comm comm,
                                 ps_search_task_t **master_task)
{
    ps_status_t rv = PS_SUCCESS;
    unsigned long total_filesize = 0, search_range_size = 0, master_search_range_size = 0;
    ps_search_task_t **slave_tasks = NULL;
    unsigned int path_len = strlen(path), number_of_slaves = number_of_procs - 1;
    unsigned int i = 0;
    size_t search_task_mem_size = 0;
    MPI_Request *requests = NULL;

    log_debug("%s:begin. number_of_procs:%d number_of_slaves:%d", __func__, number_of_procs, number_of_slaves);

    if (path == NULL || number_of_procs < 1 || *master_task != NULL)
    {
        log_err("wrong arguments in distribute_path_and_search_range: \
                 path=%s, number_of_procs:%d, master_task:%p",
                path, number_of_procs, *master_task);
        rv = PS_ERROR_WRONG_ARGUMENTS;
        goto error;
    }

    search_task_mem_size = sizeof(char) * path_len + sizeof(ps_search_task_t);

    PS_CHECK_GOTO_ERROR(get_filesize(path, &total_filesize));

    search_range_size = total_filesize / number_of_procs;

    /*The master_task is the last and a little bit bigger because it includes the rest of the division*/
    master_search_range_size = search_range_size + (total_filesize % number_of_procs);
    log_debug("search_range_size : %lu, master_search_range_size: %lu",
              search_range_size, master_search_range_size);

    /*Set search_task for master*/
    PS_CHECK_GOTO_ERROR( ps_searcher_task_create(master_task,
                         number_of_slaves * search_range_size,
                         master_search_range_size,
                         path_len, path));

    PS_MALLOC(slave_tasks, sizeof(ps_search_task_t*) * number_of_slaves);

    /*Need two requests per slave*/
    PS_MALLOC(requests, 2 * sizeof(MPI_Request) * number_of_slaves);
    /*Tell the slaves in which ranges and which files to search */
    for (i = 0; i < number_of_slaves; i++)
    {
        PS_CHECK_GOTO_ERROR( ps_searcher_task_create(&slave_tasks[i],
                             i * search_range_size,
                             search_range_size,
                             path_len,
                             path));

        log_debug("Process %d: offset:%lu, size:%lu, path:%s, search_task_mem_size:%d",
                  slave_proc_numbers[i], slave_tasks[i]->offset, slave_tasks[i]->size, slave_tasks[i]->path, search_task_mem_size);
        PS_MPI_CHECK_ERR(MPI_Isend(&path_len, 1, MPI_UNSIGNED, slave_proc_numbers[i],
                                   PS_MPI_TAG_PATH_LENGTH, comm, &requests[2 * i]));
        PS_MPI_CHECK_ERR(MPI_Isend(slave_tasks[i], search_task_mem_size, MPI_BYTE, slave_proc_numbers[i],
                                   PS_MPI_TAG_SEARCH_TASK, comm, &requests[2 * i + 1]));
    }

    log_debug("Waiting for &d slaves", number_of_slaves);
    PS_MPI_CHECK_ERR(MPI_Waitall(2 * number_of_slaves, requests, MPI_STATUSES_IGNORE));
    log_debug("Waiting for slaves done");
    for (i = 0; i < number_of_slaves; i++)
    {
        PS_FREE(slave_tasks[i]);
    }

    PS_FREE(slave_tasks);
    PS_FREE(requests);
    log_debug("%s:end", __func__);

    return rv;

error:
    log_debug("%s:error", __func__);
    PS_FREE(*master_task);
    if (slave_tasks)
    {
        for (i = 0; i < number_of_slaves; i++)
        {
            PS_FREE(slave_tasks[i]);
        }
    }

    PS_FREE(slave_tasks);
    PS_FREE(requests);

    return rv;
}

int
recv_task(ps_search_task_t **task,
          int own_rank,
          int master,
          MPI_Comm comm)
{
    ps_status_t rv = PS_SUCCESS;
    MPI_Status status;
    unsigned int path_length = -1;
    size_t tasklen = 0;

    log_debug("%s:begin", __func__);

    PS_MPI_CHECK_ERR(MPI_Recv(&path_length, 1, MPI_UNSIGNED, master, PS_MPI_TAG_PATH_LENGTH, comm, &status));

    tasklen = sizeof(char) * path_length + sizeof(ps_search_task_t);
    log_debug("Process %d: path_length:%d ,tasklen:%d", own_rank, path_length, tasklen);
    PS_MALLOC(task, tasklen);
    PS_MPI_CHECK_ERR(MPI_Recv(task, tasklen, MPI_BYTE, master, PS_MPI_TAG_SEARCH_TASK, comm, &status));

    log_debug("%s:end", __func__);
    return rv;

error:
    log_debug("%s:error", __func__);
    PS_FREE(task);

    return rv;
}

