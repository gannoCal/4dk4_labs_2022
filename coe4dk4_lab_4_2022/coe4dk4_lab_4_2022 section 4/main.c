
/*
 * Simulation_Run of the ALOHA Protocol
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "output.h"
#include "trace.h"
#include "simparameters.h"
#include "cleanup.h"
#include "packet_arrival.h"
#include "packet_transmission.h"
#include "main.h"

/*******************************************************************************/

int
main(void)
{
  /* Get the list of random number generator seeds defined in simparameters.h */
  unsigned random_seed;
  unsigned RANDOM_SEEDS[] = {RANDOM_SEED_LIST, 0};

  Simulation_Run_Ptr simulation_run;
  Simulation_Run_Data data;
  int i, j=0;

  //calc arrival rates / mean_call_time
  double arrival_rates[500];
  double arrival_rate_min = 0.001;
  int arrival_rate_data_points = 150;
  double arrival_rate_max = 1.5;
  for (int i = 0; i < arrival_rate_data_points; i++) {
      if (i == 0) arrival_rates[i] = arrival_rate_min;
      else arrival_rates[i] = arrival_rates[i - 1] + (double)((double)(arrival_rate_max - arrival_rate_min) / (arrival_rate_data_points - 1));
  }
  arrival_rates[arrival_rate_data_points] = -1;


  //calc mean_backoff_duration
  long int mean_backoff_duration_arr[100];
  long int mean_backoff_duration_min = 3;
  long int mean_backoff_duration_data_points = 3;
  long int mean_backoff_duration_delta = 2;
  for (int i = 0; i < mean_backoff_duration_data_points; i++) {
      if (i == 0) mean_backoff_duration_arr[i] = mean_backoff_duration_min;
      else mean_backoff_duration_arr[i] = mean_backoff_duration_arr[i - 1] + mean_backoff_duration_delta;
  }
  mean_backoff_duration_arr[mean_backoff_duration_data_points] = -1;

  //calc number_of_stations
  long int number_of_stations_arr[100];
  long int number_of_stations_min = 5;
  long int number_of_stations_data_points = 2;
  long int number_of_stations_delta = 5;
  for (int i = 0; i < number_of_stations_data_points; i++) {
      if (i == 0) number_of_stations_arr[i] = number_of_stations_min;
      else number_of_stations_arr[i] = number_of_stations_arr[i - 1] + number_of_stations_delta;
  }
  number_of_stations_arr[number_of_stations_data_points] = -1;

  int k = 0;
  int l = 0;
  int h = 0;


  int EXP_OR_FIXED_B = 1;

  if (EXP_OR_FIXED_B) {
      mean_backoff_duration_arr[1] = -1;
      mean_backoff_duration_data_points = 1;
  }


#define declare_station(id) double* mean_delay_##id = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double)); \
  double* throughput_##id = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double)); \
  double* G_rate_##id = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double));

  declare_station(s0)
      declare_station(s1)
      declare_station(s2)
      declare_station(s3)
      declare_station(s4)
      declare_station(s5)
      declare_station(s6)
      declare_station(s7)
      declare_station(s8)
      declare_station(s9)

  double* mean_delay_rest = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double));
  double* throughput_rest = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double));
  double* G_rate_rest = malloc(mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points * sizeof(double));

  for (int i = 0; i < mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points; i++) {
      
#define prep_station(id)      mean_delay_##id[i] = -1; \
      throughput_##id[i] = -1; \
      G_rate_##id[i] = -1; \

      prep_station(s0)
          prep_station(s1)
          prep_station(s2)
          prep_station(s3)
          prep_station(s4)
          prep_station(s5)
          prep_station(s6)
          prep_station(s7)
          prep_station(s8)
          prep_station(s9)
        
      mean_delay_rest[i] = -1;
      throughput_rest[i] = -1;
      G_rate_rest[i] = -1;

  }

  

  data.runs_required = mean_backoff_duration_data_points * number_of_stations_data_points * arrival_rate_data_points;
  data.run_count = 0;

  int single_run = 0;
  

  /* Do a new simulation_run for each random number generator seed. */
  while ((random_seed = RANDOM_SEEDS[j++]) != 0) {
      k = 0;
      while ((mean_backoff_duration_arr[k++]) != -1) {
          l = 0;
          while ((number_of_stations_arr[l++]) != -1) {
              h = 0;
              while ((arrival_rates[h++]) != -1) {

                  if (!EXP_OR_FIXED_B) {
                      if (number_of_stations_arr[l - 1] == 5) {
                          mean_backoff_duration_arr[0] = 3;
                          mean_backoff_duration_arr[1] = 5;
                          mean_backoff_duration_arr[2] = 7;
                      }
                      if (number_of_stations_arr[l - 1] == 10) {
                          mean_backoff_duration_arr[0] = 5;
                          mean_backoff_duration_arr[1] = 10;
                          mean_backoff_duration_arr[2] = 20;

                      }
                  }
                  

                  int index3d = (k - 1) + ((l - 1) * mean_backoff_duration_data_points) + ((h - 1) * mean_backoff_duration_data_points * number_of_stations_data_points);


                  /* Set the random generator seed. */
                  random_generator_initialize(random_seed);

                  /* Create a new simulation_run. This gives a clock and
                     eventlist. Clock time is set to zero. */
                  simulation_run = (Simulation_Run_Ptr)simulation_run_new();

                  /* Add our data definitions to the simulation_run. */
                  simulation_run_set_data(simulation_run, (void*)&data);

                  /* Create and initalize the stations. */
                  data.stations = (Station_Ptr)xcalloc((unsigned int)number_of_stations_arr[l - 1],
                      sizeof(Station));

                  /* Initialize various simulation_run variables. */
                  data.blip_counter = 0;
                  data.arrival_count = 0;
                  data.number_of_packets_processed = 0;
                  data.number_of_collisions = 0;
                  data.accumulated_delay = 0.0;
                  data.random_seed = random_seed;
                  data.mean_backoff_duration = mean_backoff_duration_arr[k - 1];
                  data.number_of_stations = number_of_stations_arr[l - 1];
                  data.arrival_rate = number_of_stations_arr[l - 1] == 10 ? arrival_rates[h - 1] :  arrival_rates[h - 1];
                  data.run_count += 1;
                  data.EXP_OR_FIXED_BACKOFF = EXP_OR_FIXED_B;
                  data.successful_transmit_time = 0;
                  data.attempted_transmissions = 0;

                      /* Initialize the stations. */
                      for (i = 0; i < number_of_stations_arr[l - 1]; i++) {
                          (data.stations + i)->id = i;
                          (data.stations + i)->buffer = fifoqueue_new();
                          (data.stations + i)->packet_count = 0;
                          (data.stations + i)->accumulated_delay = 0.0;
                          (data.stations + i)->mean_delay = 0;
                          (data.stations + i)->successful_transmit_time = 0;
                          (data.stations + i)->attempted_transmissions = 0;
                      }

                  /* Create and initialize the channel. */
                  data.channel = channel_new();

                  /* Schedule initial packet arrival. */
                  schedule_packet_arrival_event(simulation_run,
                      simulation_run_get_time(simulation_run) +
                      exponential_generator((double)1 / data.arrival_rate));

                  /* Execute events until we are finished. */
                  while (data.number_of_packets_processed < RUNLENGTH && (data.number_of_packets_processed == 0 || ((data.number_of_packets_processed > 0) && ((data.accumulated_delay / data.number_of_packets_processed) < 5e5)))) {
                      simulation_run_execute_event(simulation_run);
                  }

                  /* Print out some results. */
                  output_results(simulation_run);

                  
                  

                  mean_delay_rest[index3d] = data.accumulated_delay / data.number_of_packets_processed;
                  throughput_rest[index3d] = data.successful_transmit_time / simulation_run->clock->time;
                  G_rate_rest[index3d] = data.attempted_transmissions / simulation_run->clock->time;

                  #define   update_station(id)               mean_delay_s##id[index3d] = (data.stations + id)->accumulated_delay / (data.stations + id)->packet_count; \
                                                             throughput_s##id[index3d] = (data.stations + id)->successful_transmit_time / simulation_run->clock->time; \
                                                             G_rate_s##id[index3d] =     (data.stations + id)->attempted_transmissions / simulation_run->clock->time; \

                      update_station(0)
                      update_station(1)
                      update_station(2)
                      update_station(3)
                      update_station(4)
                          if (number_of_stations_arr[l - 1] == 10) {
                                  update_station(5)
                                  update_station(6)
                                  update_station(7)
                                  update_station(8)
                                  update_station(9)
                          }
                      
                   
                  /* Clean up memory. */
                  cleanup(simulation_run);
              }
          }
      }
  }

#define OUT_STR_SIZE 500000
  if (!single_run) {
      char str[OUT_STR_SIZE];
      int index = 0;
      FILE* fp;
      
#define fileout(id)        \
       for (int ii = 0; i < OUT_STR_SIZE; i++) { \
          str[ii] = 0; \
      } \
      i = 0; \
      j = 0; \
      index = 0; \
      while (number_of_stations_arr[i] != -1) { \
          if (!EXP_OR_FIXED_B) { \
              if (number_of_stations_arr[i] == 5) { \
                  mean_backoff_duration_arr[0] = 3; \
                  mean_backoff_duration_arr[1] = 5; \
                  mean_backoff_duration_arr[2] = 7; \
              } \
              if (number_of_stations_arr[i] == 10) { \
                  mean_backoff_duration_arr[0] = 5; \
                  mean_backoff_duration_arr[1] = 10; \
                  mean_backoff_duration_arr[2] = 20; \
              } \
          } \
          j = 0; \
          while (mean_backoff_duration_arr[j] != -1) { \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%d,%d)\",", mean_backoff_duration_arr[j], number_of_stations_arr[i]); \
              j++; \
          } \
          i++; \
      } \
      if (index > 0) str[index - 1] = 0; \
      printf("rates_s = %s \n", str); \
 \
       \
      fp = fopen("mean_delay_" #id ".csv", "w"); \
      fprintf(fp, "%s,%s,%s\n", \
          "Seed", \
          "arrival rate", \
          str \
 \
      ); \
 \
 \
      i = 0; \
      j = 0; \
      index = 0; \
      l = 0; \
      h = 0; \
      while ((RANDOM_SEEDS[l]) != 0) { \
          k = 0; \
          j = 0; \
          while (arrival_rates[k] != -1) { \
              j = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]); \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[k]); \
              while (number_of_stations_arr[j] != -1) { \
                  i = 0; \
                  while (mean_backoff_duration_arr[i] != -1) { \
                      int index3d = i + (j * mean_backoff_duration_data_points) + (k * mean_backoff_duration_data_points * number_of_stations_data_points); \
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", mean_delay_##id[index3d]); \
                      i++; \
                  } \
                  j++; \
              } \
 \
              if (index > 0) str[--index] = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n"); \
              k++; \
          } \
          l++; \
 \
      } \
 \
       \
 \
 \
 \
 \
 \
      fprintf(fp, "%s\n", \
 \
 \
          str \
      ); \
      fflush(fp); \
 \
 \
       \
      for (int ii = 0; i < OUT_STR_SIZE; i++) { \
          str[ii] = 0; \
      } \
 \
      i = 0; \
      j = 0; \
      index = 0; \
      while (number_of_stations_arr[i] != -1) { \
          if (!EXP_OR_FIXED_B) { \
              if (number_of_stations_arr[i] == 5) { \
                  mean_backoff_duration_arr[0] = 3; \
                  mean_backoff_duration_arr[1] = 5; \
                  mean_backoff_duration_arr[2] = 7; \
              } \
              if (number_of_stations_arr[i] == 10) { \
                  mean_backoff_duration_arr[0] = 5; \
                  mean_backoff_duration_arr[1] = 10; \
                  mean_backoff_duration_arr[2] = 20; \
              } \
          } \
          j = 0; \
          while (mean_backoff_duration_arr[j] != -1) { \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%d,%d)\",", mean_backoff_duration_arr[j], number_of_stations_arr[i]); \
              j++; \
          } \
          i++; \
      } \
      if (index > 0) str[index - 1] = 0; \
      printf("rates_s = %s \n", str); \
 \
       \
      fp = fopen("throughput_" #id ".csv", "w"); \
      fprintf(fp, "%s,%s,%s\n", \
          "Seed", \
          "arrival rate", \
          str \
 \
      ); \
 \
 \
      i = 0; \
      j = 0; \
      index = 0; \
      l = 0; \
      h = 0; \
      while ((RANDOM_SEEDS[l]) != 0) { \
          k = 0; \
          j = 0; \
          while (arrival_rates[k] != -1) { \
              j = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]); \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[k]); \
              while (number_of_stations_arr[j] != -1) { \
                  i = 0; \
                  while (mean_backoff_duration_arr[i] != -1) { \
                      int index3d = i + (j * mean_backoff_duration_data_points) + (k * mean_backoff_duration_data_points * number_of_stations_data_points); \
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", throughput_##id[index3d]); \
                      i++; \
                  } \
                  j++; \
              } \
 \
              if (index > 0) str[--index] = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n"); \
              k++; \
          } \
          l++; \
 \
      } \
 \
      fprintf(fp, "%s\n", \
 \
 \
          str \
      ); \
      fflush(fp); \
 \
       \
      for (int ii = 0; i < OUT_STR_SIZE; i++) { \
          str[ii] = 0; \
      } \
 \
      i = 0; \
      j = 0; \
      index = 0; \
      while (number_of_stations_arr[i] != -1) { \
          if (!EXP_OR_FIXED_B) { \
              if (number_of_stations_arr[i] == 5) { \
                  mean_backoff_duration_arr[0] = 3; \
                  mean_backoff_duration_arr[1] = 5; \
                  mean_backoff_duration_arr[2] = 7; \
              } \
              if (number_of_stations_arr[i] == 10) { \
                  mean_backoff_duration_arr[0] = 5; \
                  mean_backoff_duration_arr[1] = 10; \
                  mean_backoff_duration_arr[2] = 20; \
              } \
          } \
          j = 0; \
          while (mean_backoff_duration_arr[j] != -1) { \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%d,%d)\",", mean_backoff_duration_arr[j], number_of_stations_arr[i]); \
              j++; \
          } \
          i++; \
      } \
      if (index > 0) str[index - 1] = 0; \
      printf("rates_s = %s \n", str); \
 \
 \
      fp = fopen("G_rate_" #id ".csv", "w"); \
      fprintf(fp, "%s,%s,%s\n", \
          "Seed", \
          "arrival rate", \
          str \
 \
      ); \
 \
 \
      i = 0; \
      j = 0; \
      index = 0; \
      l = 0; \
      h = 0; \
      while ((RANDOM_SEEDS[l]) != 0) { \
          k = 0; \
          j = 0; \
          while (arrival_rates[k] != -1) { \
              j = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]); \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[k]); \
              while (number_of_stations_arr[j] != -1) { \
                  i = 0; \
                  while (mean_backoff_duration_arr[i] != -1) { \
                      int index3d = i + (j * mean_backoff_duration_data_points) + (k * mean_backoff_duration_data_points * number_of_stations_data_points); \
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", G_rate_##id[index3d]); \
                      i++; \
                  } \
                  j++; \
              } \
 \
              if (index > 0) str[--index] = 0; \
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n"); \
              k++; \
          } \
          l++; \
 \
      } \
 \
      fprintf(fp, "%s\n", \
 \
 \
          str \
      ); \
      fflush(fp);


fileout(s0)
fileout(s1)
fileout(s2)
fileout(s3)
fileout(s4)
fileout(s5)
fileout(s6)
fileout(s7)
fileout(s8)
fileout(s9)
fileout(rest)






      

      
  }

#define station_free(id) xfree(mean_delay_##id); xfree(throughput_##id); xfree(G_rate_##id);

  station_free(s0)
      station_free(s1)
      station_free(s2)
      station_free(s3)
      station_free(s4)
      station_free(s5)
      station_free(s6)
      station_free(s7)
      station_free(s8)
      station_free(s9)
      station_free(rest)

  /* Pause before finishing. */
  getchar();

  return 0;
}












