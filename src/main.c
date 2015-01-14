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
    char *search = NULL, *filename = NULL;
    int i = 0, c = 0;
    int number_of_procs = 0, own_rank = 0;
    ps_search_task_t *task = NULL;
    int *slave_procs = NULL;

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
                filename = optarg;
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
        log_debug("search = %s, filename = %s", search, filename);

        for (i = optind; i < argc; i++)
        {
            log_debug("Non-option argument %s", argv[i]);
        }
        /*-------------------Processing of arguments done!-----------*/
    }

    PS_MPI_CHECK_ERR(MPI_Bcast(&log_level, 1, MPI_INT, MASTER, MPI_COMM_WORLD));
            
    /*Communicate log_level*/
    if(own_rank == MASTER)
    {
        PS_MALLOC( slave_procs, sizeof(int) * (number_of_procs - 1));
        for (i = 0; i < number_of_procs - 1; i++)
        {
            slave_procs[i] = i + 1;
        }


        PS_CHECK_GOTO_ERROR( distribute_filename_and_search_range(filename, number_of_procs - 1,
                             slave_procs, MPI_COMM_WORLD, &task));
    }
    else
    {
        /*Slaves receive filename_length and search_task*/
        set_log_level(log_level);

        PS_CHECK_GOTO_ERROR( recv_task(&task, own_rank, MASTER, MPI_COMM_WORLD));
        PS_CHECK_GOTO_ERROR( ps_file_searcher_search(task));
    }

    log_debug("Process %d finished", own_rank);

    MPI_Finalize();

    PS_FREE(slave_procs);
    PS_FREE(task);
    return EXIT_SUCCESS;
    /*-----------------ERROR-Handling------------------------------*/
error:

    MPI_Finalize();
    PS_FREE(slave_procs);
    PS_FREE(task);
    return rv;
}

