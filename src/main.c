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

int
main(int argc, char *argv[])
{
    ps_status_t rv = PS_SUCCESS;
    int log_level = LOG_LEVEL_NONE;
    char *search = NULL, *filename = NULL;
    int i = 0, c = 0;
    int number_of_procs = 0, own_rank = 0;
    FILE *searchfile = NULL;
    search_task_t *task = NULL;

    out_fd = stdout; /*For Logging*/

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_procs);
    MPI_Comm_rank (MPI_COMM_WORLD, &own_rank);

    if (own_rank == MASTER)
    {
        long total_filesize = -1, search_range_size = -1;

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

                rv = PS_WRONG_ARGUMENTS;
                goto error;
            }
        }

        set_log_level(LOG_LEVEL_DEBUG);
        log_debug("search = %s, filename = %s\n", search, filename);

        for (i = optind; i < argc; i++)
        {
            log_debug("Non-option argument %s\n", argv[i]);
        }
        /*-------------------Processing of arguments done!-----------*/

        /*----Calc the total filesize of the file, that should be analyzed---*/
        PS_CHECK_PTR_NULL((searchfile = fopen(filename, "r")), PS_FAILED_TO_OPEN_FILE);
        PS_CHECK_GOTO_ERROR(fseek(searchfile, 0L, SEEK_END)); 
        PS_CHECK_NEG_RET_AND_GOTO_ERROR((total_filesize = ftell(searchfile)));
        log_debug("Total filesize: %lu", total_filesize);
        PS_CLOSE_FILE(searchfile);
    }

    log_debug("Process %d finished\n", own_rank);
    MPI_Finalize();

    return EXIT_SUCCESS;
    /*-----------------ERROR-Handling------------------------------*/
error:

    MPI_Finalize();
    PS_CLOSE_FILE(searchfile); 
    return rv;
}

