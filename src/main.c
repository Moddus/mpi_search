#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>
#include <sys/time.h>

#include "util.h"
#include "log.h"
#include "mpi_functions.h"
#include "ps_mpi.h"
#include "csv.h"

#ifdef TIME_MEASUREMENT
#include "timeutil.h"

extern float process_search_time;
extern float process_file_io_time;
#endif

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
    size_t result_len = 0, total_result_len = 0, *all_result_len = NULL;
    int search_col = PS_CSV_ALL_COL;

#ifdef TIME_MEASUREMENT
    float total_seconds = 0, total_search_time = 0, total_file_io_time = 0, total_setup_time = 0, total_reduce_time = 0;
    struct timeval time_start, current_time;
    gettimeofday(&time_start, NULL);
    memcpy(&current_time, &time_start, sizeof(struct timeval));
#endif

    out_fd = stdout; /*For Logging*/

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_procs);
    MPI_Comm_rank (MPI_COMM_WORLD, &own_rank);

    /*set log level until arguments are passed and processed*/
    set_log_level(log_level);

    if (own_rank == MASTER)
    {
        opterr = 0;
        while ((c = getopt (argc, argv, "hds:f:c:l:")) != -1)
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
            case 'l':
                search_col = atoi(optarg);
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

#ifdef TIME_MEASUREMENT
    if(own_rank == MASTER)
    {
        update_timestamp_and_total_seconds(&current_time, &total_setup_time);
    }
#endif

    if (own_rank == MASTER)
    {
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

        PS_CHECK_GOTO_ERROR( distribute_path_and_search_range(path,
                                                              number_of_procs ,
                                                              slave_nodes,
                                                              chunk_size,
                                                              search_col,
                                                              MPI_COMM_WORLD,
                                                              &task));

        /*Slaves receive path_length and search_task*/

        PS_CHECK_GOTO_ERROR(ps_file_searcher_create(&searcher, search, task));
        PS_CHECK_GOTO_ERROR(ps_file_searcher_search(searcher, &result, &result_len));
        log_debug("Process %d: result_len: %lu", own_rank, result_len);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_free(&searcher));
        PS_MALLOC(all_result_len, sizeof(size_t) * number_of_procs);
    }
    else
    {
        /*Slaves receive path_length and search_task*/
        PS_CHECK_GOTO_ERROR(recv_task(&task, own_rank, MASTER, MPI_COMM_WORLD));

        PS_CHECK_GOTO_ERROR(ps_file_searcher_create(&searcher, search, task));
        PS_CHECK_GOTO_ERROR(ps_file_searcher_search(searcher, &result, &result_len));
        log_debug("Process %d: result_len: %lu", own_rank, result_len);
        PS_CHECK_GOTO_ERROR(ps_file_searcher_free(&searcher));
    }

#ifdef TIME_MEASUREMENT
    if(own_rank == MASTER)
    {
        gettimeofday(&current_time, NULL);
    }
#endif

    PS_MPI_CHECK_ERR(MPI_Gather(&result_len, 1, MPI_UNSIGNED_LONG, all_result_len, 1, MPI_UNSIGNED_LONG, MASTER,
            MPI_COMM_WORLD));

    if(own_rank == MASTER)
    {
        for(i = 0; i < number_of_procs; i++){
            total_result_len += all_result_len[i];
        }
        log_debug("Process %d: total_result_len:%lu", own_rank, total_result_len);
        PS_REALLOC(result, total_result_len);
        for(i = 1; i < number_of_procs; i++){
            PS_MPI_CHECK_ERR(MPI_Recv(result + all_result_len[i - 1], all_result_len[i], MPI_CHAR,
                    i, PS_MPI_TAG_RESULT, MPI_COMM_WORLD, MPI_STATUSES_IGNORE));
            result_len += all_result_len[i];
        }
        write(STDOUT_FILENO, result, result_len);
    }
    else
    {
        PS_MPI_CHECK_ERR(MPI_Send(result, result_len, MPI_CHAR, MASTER, PS_MPI_TAG_RESULT, MPI_COMM_WORLD));
        PS_FREE(search);
    }

    log_debug("Process %d finished", own_rank);

#ifdef TIME_MEASUREMENT
    if(own_rank == MASTER)
    {
        update_timestamp_and_total_seconds(&current_time, &total_reduce_time);
    }
    printf("Process %d: process_search_time: %f, process_file_io_time: %f\n", own_rank
        ,process_search_time, process_file_io_time);

    PS_MPI_CHECK_ERR(MPI_Reduce(&process_search_time, &total_search_time, 1, MPI_FLOAT, MPI_SUM, MASTER,
            MPI_COMM_WORLD));
    PS_MPI_CHECK_ERR(MPI_Reduce(&process_file_io_time , &total_file_io_time, 1, MPI_FLOAT, MPI_SUM, MASTER,
            MPI_COMM_WORLD));

    update_timestamp_and_total_seconds(&time_start, &total_seconds);
    if(own_rank == MASTER)
    {
        printf("Total-Time: %.3fs\n"
                        "\ttotal_setup_time: %.3fs\n"
                        "\ttotal_reduce_time: %.3fs\n"
                        "\taverage-io-time: %.3fs\n"
                        "\taverage-search-time: %.3fs\n"
                        "processes: %d\n"
                        "chunksize: %lu Bytes\n",
                total_seconds, total_setup_time, total_reduce_time,
                total_file_io_time / number_of_procs, total_search_time / number_of_procs,
                number_of_procs, chunk_size);
    }
#endif

    MPI_Finalize();

    PS_FREE(slave_nodes);
    PS_FREE(result);
    PS_FREE(all_result_len);

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
    PS_FREE(all_result_len);

    return rv;
}

