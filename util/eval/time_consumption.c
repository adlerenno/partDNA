//
// Created by Enno Adler on 15.11.23.
//

#include "time_consumption.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

typedef struct {
    clock_t cpu_start;
    struct timespec real_time;
} StartTimes;

void *time_consumption_start()
{
    StartTimes *ret = malloc(sizeof (StartTimes));
    ret->cpu_start = clock();
    clock_gettime(CLOCK_MONOTONIC, &(ret->real_time));
    return ret;
}

void time_consumption_stop(void *started_data, double *real_elapsed_time, double *time_on_cpu)
{
    struct timespec end_real_time;
    clock_gettime(CLOCK_MONOTONIC, &end_real_time);
    clock_t end_cpu = clock();
    StartTimes *st = (StartTimes *) started_data;
    *real_elapsed_time = (end_real_time.tv_sec - st->real_time.tv_sec);
    *real_elapsed_time += (end_real_time.tv_nsec - st->real_time.tv_nsec) / 1000000000.0;
    *time_on_cpu = ((double) (end_cpu - st->cpu_start)) / CLOCKS_PER_SEC;
    free(started_data);
}