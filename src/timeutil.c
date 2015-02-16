#include <time.h>
#include <string.h>

#include "timeutil.h"

float process_search_time = 0;
float process_file_io_time = 0;

void
update_timestamp_and_total_seconds(struct timeval *last_timestamp, float *total_seconds)
{
    struct timeval current_time;
    long secs, usecs;

    gettimeofday(&current_time, NULL);
    secs = current_time.tv_sec - last_timestamp->tv_sec;
    usecs = current_time.tv_usec - last_timestamp->tv_usec;

    if(usecs < 0)
    {
        usecs += 1000000;
        secs--;
    }
    *total_seconds += secs + (usecs / 1000000.0);

    memcpy(last_timestamp, &current_time, sizeof(struct timeval));
}

