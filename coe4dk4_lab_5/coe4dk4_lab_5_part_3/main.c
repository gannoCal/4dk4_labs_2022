
/*
 * 
 * Simulation_Run of A Single Server Queueing System
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA,
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
#include "simparameters.h"
#include "packet_arrival.h"
#include "cleanup_memory.h"
#include "trace.h"
#include "main.h"

/******************************************************************************/

/*
 * main.c declares and creates a new simulation_run with parameters defined in
 * simparameters.h. The code creates a fifo queue and server for the single
 * server queueuing system. It then loops through the list of random number
 * generator seeds defined in simparameters.h, doing a separate simulation_run
 * run for each. To start a run, it schedules the first packet arrival
 * event. When each run is finished, output is printed on the terminal.
 */

int
main(void)
{
  Simulation_Run_Ptr simulation_run;
  Simulation_Run_Data data;

  /*
   * Declare and initialize our random number generator seeds defined in
   * simparameters.h
   */

  unsigned RANDOM_SEEDS[] = {RANDOM_SEED_LIST, 0};
  unsigned random_seed;
  int j=0;
  int i = 0;


  //calc arrival rates / mean_call_time
  double arrival_rates[500];
  double arrival_rate_min = 0.001;
  int arrival_rate_data_points = 100;
  double arrival_rate_max = 1.1;
  for (int i = 0; i < arrival_rate_data_points; i++) {
      if (i == 0) arrival_rates[i] = arrival_rate_min;
      else arrival_rates[i] = arrival_rates[i - 1] + (double)((double)(arrival_rate_max - arrival_rate_min) / (arrival_rate_data_points - 1));
  }
  arrival_rates[arrival_rate_data_points] = -1;



  int k = 0;


  double* mean_delay_good = malloc( arrival_rate_data_points * sizeof(double));
  double* mean_delay_bad = malloc( arrival_rate_data_points * sizeof(double));
  long int* number_processed_good = malloc(arrival_rate_data_points * sizeof(long int));
  long int* number_processed_bad = malloc(arrival_rate_data_points * sizeof(long int));


  for (int i = 0; i <  arrival_rate_data_points ; i++) {
      mean_delay_good[i] = -1;
      mean_delay_bad[i] = -1;
      number_processed_good[i] = -1;
      number_processed_bad[i] = -1;
  }

  /* 
   * Loop for each random number generator seed, doing a separate
   * simulation_run run for each.
   */

  data.runs_required = arrival_rate_data_points;
  data.run_count = 0;

  int single_run = 0;

  while ((random_seed = RANDOM_SEEDS[j++]) != 0) {
      k = 0;
      while ((arrival_rates[k++]) != -1) {

          simulation_run = simulation_run_new(); /* Create a new simulation run. */

          /*arrival_rates*/    int index3d = (k - 1);

          data.run_count++;
             

          /*
           * Set the simulation_run data pointer to our data object.
           */

          simulation_run_attach_data(simulation_run, (void*)&data);

          /*
           * Initialize the simulation_run data variables, declared in main.h.
           */

          data.blip_counter = 0;
          data.arrival_count = 0;
          data.number_of_packets_processed = 0;
          data.number_of_packets_processed_good = 0;
          data.number_of_packets_processed_bad = 0;
          data.accumulated_delay_good = 0.0;
          data.accumulated_delay_bad = 0.0;
          data.random_seed = random_seed;
          data.number_of_packets_processed_base = 0;
          data.arrival_rate = arrival_rates[k - 1];

          /*
           * Create the packet buffer and transmission link, declared in main.h.
           */

          data.cloud_server_buffer = fifoqueue_new();
          data.good_phone_buffer = fifoqueue_new();
          data.bad_phone_buffer = fifoqueue_new();

          data.cloud_server = server_new();
          data.good_phone_server = server_new();
          data.bad_phone_server = server_new();


          /*
           * Set the random number generator seed for this run.
           */

          random_generator_initialize(random_seed);

          /*
           * Schedule the initial packet arrival for the current clock time (= 0).
           */

          schedule_packet_arrival_event(simulation_run,
              simulation_run_get_time(simulation_run));

          /*
           * Execute events until we are finished.
           */

          while (data.number_of_packets_processed < RUNLENGTH) {
              simulation_run_execute_event(simulation_run);
          }


          mean_delay_good[index3d] = data.accumulated_delay_good / data.number_of_packets_processed_good;
          mean_delay_bad[index3d] = data.accumulated_delay_bad / data.number_of_packets_processed_bad;
          number_processed_good[index3d] = data.number_of_packets_processed_good;
          number_processed_bad[index3d] = data.number_of_packets_processed_bad;

          /*
           * Output results and clean up after ourselves.
           */

          output_results(simulation_run);
          cleanup_memory(simulation_run);
      }
  }

#define OUT_STR_SIZE 500000
  int l;
  if (!single_run) {
      //file1
      char str[OUT_STR_SIZE];
      i = 0;
      j = 0;
      k = 0;
      
      int index = 0;
      
      index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%s)\",", "Good");
      index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%s)\",", "Bad");
      if (index > 0) str[index - 1] = 0;
      printf("rates_s = %s \n", str);

      FILE* fp;
      fp = fopen("mean_delay.csv", "w");
      fprintf(fp, "%s,%s,%s\n",
          "Seed",
          "arrival rate",
          str

      );


      i = 0;
      j = 0;
      index = 0;
      l = 0;
      
      while ((RANDOM_SEEDS[l]) != 0) {
          k = 0;
          j = 0;
          while (arrival_rates[k] != -1) {
              j = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[k]);
              
              /*arrival_rates*/     int index3d = (k);
                              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", mean_delay_good[index3d]);
                              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", mean_delay_bad[index3d]);
                              

              if (index > 0) str[--index] = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n");
              k++;
          }
          l++;

      }


      fprintf(fp, "%s\n",


          str
      );
      fflush(fp);


      //file2
      for (int ii = 0; i < OUT_STR_SIZE; i++) {
          str[ii] = 0;
      }
      i = 0;
      j = 0;
      k = 0;

      index = 0;

      index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%s)\",", "Good");
      index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%s)\",", "Bad");
      if (index > 0) str[index - 1] = 0;
      printf("rates_s = %s \n", str);

      
      fp = fopen("number_processed.csv", "w");
      fprintf(fp, "%s,%s,%s\n",
          "Seed",
          "arrival rate",
          str

      );


      i = 0;
      j = 0;
      index = 0;
      l = 0;

      while ((RANDOM_SEEDS[l]) != 0) {
          k = 0;
          j = 0;
          while (arrival_rates[k] != -1) {
              j = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[k]);

              /*arrival_rates*/     int index3d = (k);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%0d,", number_processed_good[index3d]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%0d,", number_processed_bad[index3d]);


              if (index > 0) str[--index] = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n");
              k++;
          }
          l++;

      }


      fprintf(fp, "%s\n",


          str
      );
      fflush(fp);
      
  }

  getchar();   /* Pause before finishing. */
  return 0;
}












