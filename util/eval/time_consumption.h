//
// Created by Enno Adler on 15.11.23.
//

#ifndef BWT_TIME_CONSUMPTION_H
#define BWT_TIME_CONSUMPTION_H

#include <stdint.h>

void *time_consumption_start();
void time_consumption_stop(void *started_data, double *real_elapsed_time, double *time_on_cpu);
#endif //BWT_TIME_CONSUMPTION_H
