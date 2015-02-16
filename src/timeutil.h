#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <sys/time.h>

void
update_timestamp_and_total_seconds(struct timeval *last_timestamp, float *total_seconds);

#endif
