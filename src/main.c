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
    int i = 0, c = 0;
    int number_of_procs = 0, own_rank = 0;
    ps_search_task_t *task = NULL;
    int *slave_nodes = NULL;

    out_fd = stdout; /*For Logging*/

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_procs);
    MPI_Comm_rank (MPI_COMM_WORLD, &own_rank);

    /*set log level until arguments are passed and processed*/
    set_log_level(log_level);

    if (own_rank == MASTER)
    {
        opterr = 0;
        while ((c = getopt (argc, argv, "hds:f:")) != -1)
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
                break;
            case 'h':
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

        set_log_level(log_level);
        log_debug("search = %s, path = %s", search, path);

        for (i = optind; i < argc; i++)
        {
            log_debug("Non-option argument %s", argv[i]);
        }
        /*-------------------Processing of arguments done!-----------*/
    }

    /*Communicate log_level*/
    PS_MPI_CHECK_ERR(MPI_Bcast(&log_level, 1, MPI_INT, MASTER, MPI_COMM_WORLD));

    if (own_rank == MASTER)
    {
        log_debug("Number of procs:%d", number_of_procs);
        PS_MALLOC( slave_nodes, sizeof(int) * (number_of_procs));
        for (i = 0; i < number_of_procs - 1; i++)
        {
            slave_nodes[i] = i + 1;
        }

        PS_CHECK_GOTO_ERROR( distribute_path_and_search_range(path, number_of_procs ,
                             slave_nodes, MPI_COMM_WORLD, &task));
    }
    else
    {
        /*Slaves receive path_length and search_task*/
        set_log_level(log_level);
        ps_searcher_t *searcher; 
        char *result = NULL;

        PS_CHECK_GOTO_ERROR( recv_task(&task, own_rank, MASTER, MPI_COMM_WORLD));

        log_debug("vorher:%d", own_rank);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_create(&searcher, "SSL", task));
        log_debug("nachher:%d", own_rank);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_search(searcher, &result));
        PS_CHECK_GOTO_ERROR(ps_file_searcher_free(&searcher));
//        log_debug("%d:result:%s", own_rank, result);
    }

    log_debug("Process %d finished", own_rank);

    /*TODO: Entfernen*/
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

//    PS_FREE(slave_nodes);
//    PS_FREE(task);
    return EXIT_SUCCESS;
    /*-----------------ERROR-Handling------------------------------*/
error:

    MPI_Finalize();
    PS_FREE(slave_nodes);
    PS_FREE(task);
    return rv;
}

