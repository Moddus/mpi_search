#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <mpi.h>

#include "util.h"
#include "log.h"
#include "mpi_functions.h"
#include "file_searcher.h"
#include "file_util.h"
#include "ps_mpi.h"

int
main(int argc, char *argv[])
{
    ps_status_t rv = PS_SUCCESS;
    int log_level = LOG_LEVEL_NONE;
    char *search = NULL, *path = NULL;
    size_t search_len = 0;
    int i = 0, c = 0;
    int number_of_procs = 0, own_rank = 0;
    int *slave_nodes = NULL;
    unsigned long chunk_size = DEFAULT_CHUNK_SIZE;
    ps_searcher_t *searcher = NULL;
    ps_search_task_t *task = NULL;
    char *result = NULL;

    out_fd = stdout; /*For Logging*/

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_procs);
    MPI_Comm_rank (MPI_COMM_WORLD, &own_rank);

    /*set log level until arguments are passed and processed*/
    set_log_level(log_level);

    if (own_rank == MASTER)
    {
        opterr = 0;
        while ((c = getopt (argc, argv, "hds:f:c:")) != -1)
        {
            switch (c)
            {
            case 'd':
                log_level = LOG_LEVEL_DEBUG;
                break;
            case 'f':
                path = optarg;
                break;
            case 's':
                search = optarg;
                search_len = strlen(search);
                break;
            case 'c':
                PS_CHECK_ZERO_GO_ERR( (chunk_size = atol(optarg)), PS_ERROR_WRONG_CHUNK_SIZE);
                break;
            case 'h':
                break;
            default:
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n", optopt);

                rv = PS_ERROR_WRONG_ARGUMENTS;
                goto error;
            }
        }
    }

    /*Communicate and set log_level*/
    PS_MPI_CHECK_ERR(MPI_Bcast(&log_level, 1, MPI_INT, MASTER, MPI_COMM_WORLD));
    set_log_level(log_level);

    /*Communicate the token to search for*/
    PS_MPI_CHECK_ERR(MPI_Bcast(&search_len, 1, MPI_UNSIGNED_LONG, MASTER, MPI_COMM_WORLD));
    if (own_rank != MASTER)
    {
        PS_MALLOC(search, sizeof(char) * (search_len + 1));
    }
    PS_MPI_CHECK_ERR(MPI_Bcast(search, search_len + 1, MPI_CHAR, MASTER, MPI_COMM_WORLD));
    log_debug("Process %d: search_len:%u search:%s", own_rank, search_len, search);

    if (own_rank == MASTER)
    {
        char *result = NULL;
        size_t result_len = 0;
        void *temp;

        log_debug("search = %s, path = %s, chunk_size=%lu", search, path, chunk_size);
        log_debug("sizeof(ps_search_task_t:%lu", sizeof(ps_search_task_t));

        for (i = optind; i < argc; i++)
        {
            log_debug("Non-option argument %s", argv[i]);
        }

        /*-------------------Processing of arguments done!-----------*/
        log_debug("Number of procs:%d", number_of_procs);
        PS_MALLOC( slave_nodes, sizeof(int) * (number_of_procs));
        for (i = 0; i < number_of_procs - 1; i++)
        {
            slave_nodes[i] = i + 1;
        }

        PS_CHECK_GOTO_ERROR( distribute_path_and_search_range(path, number_of_procs ,
                             slave_nodes, chunk_size, MPI_COMM_WORLD, &task));

        /*Slaves receive path_length and search_task*/

        PS_CHECK_GOTO_ERROR(ps_file_searcher_create(&searcher, search, task));
        PS_CHECK_GOTO_ERROR(ps_file_searcher_search(searcher, &result, &result_len));
        log_debug("Process %d: result_len: %lu result:%s", own_rank, result_len, result);
        write(STDOUT_FILENO, result, result_len);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_free(&searcher));
    }
    else
    {
        /*Slaves receive path_length and search_task*/
        size_t result_len = 0;

        PS_CHECK_GOTO_ERROR(recv_task(&task, own_rank, MASTER, MPI_COMM_WORLD));

        PS_CHECK_GOTO_ERROR(ps_file_searcher_create(&searcher, search, task));
        PS_CHECK_GOTO_ERROR(ps_file_searcher_search(searcher, &result, &result_len));
        log_debug("Process %d: result_len: %lu result:%s", own_rank, result_len, result);
        write(STDOUT_FILENO, result, result_len);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_free(&searcher));
    }

    log_debug("Process %d finished", own_rank);

    if (own_rank != MASTER)
    {
        PS_FREE(search);
    }

    MPI_Finalize();
    PS_FREE(slave_nodes);
    PS_FREE(result);

    return EXIT_SUCCESS;
    /*-----------------ERROR-Handling------------------------------*/
error:
    log_err("Process %d finished with error: %d", own_rank, rv);
    MPI_Finalize();

    if (own_rank != MASTER)
    {
        PS_FREE(search);
    }

    PS_FREE(slave_nodes);
    if(searcher)
    {
        PS_FREE(searcher->task);
    }
    PS_FREE(result);

    return rv;
}

