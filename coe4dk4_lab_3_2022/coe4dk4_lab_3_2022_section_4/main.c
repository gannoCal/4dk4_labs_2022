
/*
 * 
 * Call Blocking in Circuit Switched Networks
 * 
 * Copyright (C) 2014 Terence D. Todd
 * Hamilton, Ontario, CANADA
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
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
#include "call_arrival.h"
#include "main.h"


/*******************************************************************************/
double fact2(double x)
{
    return tgamma(x + 1.);
}

//int index1(int x, int y, int z) {
//    return x + (y * xSize) + (z * ySize * xSize);
//}

int main(void)
{
  int i;
  

  Simulation_Run_Ptr simulation_run;
  Simulation_Run_Data data; /* Simulation_Run_Data is defined in main.h. */


  data.writes_to_file = 0;
  /* 
   * Get the list of random number generator seeds defined in simparameters.h.
   */

  unsigned RANDOM_SEEDS[] = {RANDOM_SEED_LIST, 0};
  unsigned random_seed;

  //calc number_of_channels
  long int number_of_channels[100];
  long int number_of_channels_min = 1;
  long int number_of_channels_data_points = 4;
  long int number_of_channels_delta = 4;
  for (int i = 0; i < number_of_channels_data_points; i++) {
      if (i == 0) number_of_channels[i] = number_of_channels_min;
      else number_of_channels[i] = number_of_channels[i - 1] + number_of_channels_delta;
  }
  number_of_channels[number_of_channels_data_points] = -1;

  //calc arrival rates
  double arrival_rates[500];
  double arrival_rate_min =0.1  /3;
  int arrival_rate_data_points = 20;
  double arrival_rate_max = 100.0  /3;
  for (int i = 0; i < arrival_rate_data_points; i++) {
      if (i == 0) arrival_rates[i] = arrival_rate_min;
      else arrival_rates[i] = arrival_rates[i - 1] + (double)((double)(arrival_rate_max - arrival_rate_min) / (arrival_rate_data_points - 1));
  }
  arrival_rates[arrival_rate_data_points] = -1;

  //calc hangup time
  double hangup_time[100];
  double hangup_time_min = 30.0   /60;
  int hangup_time_data_points = 3;
  double hangup_time_max = 600.0   /60;
  for (int i = 0; i < hangup_time_data_points; i++) {
      if (i == 0) hangup_time[i] = hangup_time_min;
      else hangup_time[i] = hangup_time[i - 1] + (double)((double)(hangup_time_max - hangup_time_min) / (hangup_time_data_points - 1));
  }
  hangup_time[hangup_time_data_points] = -1;

  double pb_results[500][100][3];
  double pb_results_mean[500][100];
  double pb_results_theoretical[500][100];

  //double pb_hangup[100][100][100];
  //double mean_queue_time[100][100][100];

  double* pb_hangup = malloc(500 * 100 * 100 * sizeof(double));
  double* mean_queue_time = malloc(500 * 100 * 100 * sizeof(double));
  double* mean_queue_time_no_hangup = malloc(500 * 100 * 100 * sizeof(double));


  data.runs_required = hangup_time_data_points * arrival_rate_data_points * number_of_channels_data_points;
  data.run_count = 0;

  int single_run = 0;
  double single_arrival_rate = 1.0 / 3;
  double single_hangup_time = 3.67;
  long int single_number_of_channels = 9;

  /* 
   * Loop for each random number generator seed, doing a separate
   * simulation_run run for each.
   */
  int j = 0;
  int k = 0;
  int l = 0;
  int h = 0;
  
  while ((random_seed = RANDOM_SEEDS[j++]) != 0) {
      k = 0;
      while ((arrival_rates[k++]) != -1) {
        l = 0;
        while ((number_of_channels[l++]) != -1) {
            h = 0;
            while ((hangup_time[h++]) != -1) {


                /* Create a new simulation_run. This gives a clock and eventlist. */
                simulation_run = simulation_run_new();

                /* Add our data definitions to the simulation_run. */
                simulation_run_set_data(simulation_run, (void*)&data);

                /* Initialize our simulation_run data variables. */
                data.blip_counter = 0;
                data.call_arrival_count = 0;
                data.calls_processed = 0;
                data.blocked_call_count = 0;
                data.number_of_calls_processed = 0;
                data.accumulated_call_time = 0.0;
                data.random_seed = random_seed;
                data.arrival_rate = arrival_rates[k - 1];
                data.arrival_rates = &arrival_rates[0];
                data.trunk_count = number_of_channels[l - 1];
                data.mean_hangup_time = hangup_time[h - 1];
                data.hangup_count = 0; 
                data.non_hangup_count = 0;
                data.queuing_time_accum = 0;
                data.non_queued_call_count = 0;
                data.queued_call_count = 0;
                data.hangup_count = 0;
                data.queuing_time_accum_no_hangup = 0;
                data.run_count +=1;

                if (single_run) {
                    data.arrival_rate = single_arrival_rate;
                    data.mean_hangup_time = single_hangup_time;
                    data.trunk_count = single_number_of_channels;
                }


                data.arrival_rate = arrival_rates[k - 1];
                data.buffer = fifoqueue_new();

                /*double* arrival_rates1;
                arrival_rates1 = &arrival_rates[0];
                printf("arrival rate = %.5f \n", arrival_rates1[0]);
                printf("arrival rate = %.5f \n", data.arrival_rates[0]);*/

                /* Create the channels. */
                data.channels = (Channel_Ptr*)xcalloc((int)data.trunk_count,
                    sizeof(Channel_Ptr));

                /* Initialize the channels. */
                for (i = 0; i < data.trunk_count; i++) {
                    *(data.channels + i) = server_new();
                }

                /* Set the random number generator seed. */
                random_generator_initialize((unsigned)random_seed);

                /* Schedule the initial call arrival. */
                schedule_call_arrival_event(simulation_run,
                    simulation_run_get_time(simulation_run) +
                    exponential_generator((double)1 / data.arrival_rate));

                /* Execute events until we are finished. */
                while (data.number_of_calls_processed < RUNLENGTH) {
                    simulation_run_execute_event(simulation_run);
                }

                /* Print out some results. */
                output_results(simulation_run);

                double xmtted_fraction = (double)(data.call_arrival_count -
                    data.blocked_call_count) / data.call_arrival_count;

                double offered_load = MEAN_CALL_DURATION * data.arrival_rate;

                pb_results[k - 1][l - 1][j - 1] = 1 - xmtted_fraction;
                pb_results_mean[k - 1][l - 1] = pb_results_mean[k - 1][l - 1] + pb_results[k - 1][l - 1][j - 1];

                //theoretical_erlang_B
                double accum = 0;
                for (int i = 0; i <= data.trunk_count; i++) {
                    accum += (double)pow(offered_load, i) / fact2(i);
                }
                double theoretical_erlang_B = ((double)pow(offered_load, data.trunk_count) / fact2(data.trunk_count)) / accum;

                /*printf("theoretical_erlang_B222 = %.05f \n", theoretical_erlang_B);
                printf("accum = %.05f \n", accum);
                printf("data.trunk_count = %d \n", data.trunk_count);
                printf("offered_load = %.05f \n", offered_load);*/
                pb_results_theoretical[k - 1][l - 1] = theoretical_erlang_B;

                if (data.queued_call_count > 0) mean_queue_time[(k - 1) + ((l - 1) * 500) + ((h - 1) * 500 * 100)] = data.queuing_time_accum / data.queued_call_count;
                else mean_queue_time[(k - 1) + ((l - 1) * 500) + ((h - 1) * 500 * 100)] = 0;
                if (data.non_hangup_count > 0) mean_queue_time_no_hangup[(k - 1) + ((l - 1) * 500) + ((h - 1) * 500 * 100)] = data.queuing_time_accum_no_hangup / data.non_hangup_count;
                else mean_queue_time_no_hangup[(k - 1) + ((l - 1) * 500) + ((h - 1) * 500 * 100)] = 0;
                pb_hangup[(k - 1) + ((l - 1) * 500) + ((h - 1) * 500 * 100)] = (double)data.hangup_count / data.call_arrival_count;

                /* Clean up memory. */
                cleanup(simulation_run);

                if (single_run) break;
            }
            if (single_run) break;
          }
        if (single_run) break;
      }
      if (single_run) break;
  }

  //final output to csv
   
#define OUT_STR_SIZE 500000
  if (!single_run) {
      char str[OUT_STR_SIZE];
      i = 0;
      j = 0;
      int index = 0;
      while (number_of_channels[i] != -1) {
          j = 0;
          while (hangup_time[j] != -1) {
              index += snprintf(&str[index], OUT_STR_SIZE - index, "\"(%.02f,%d)\",", hangup_time[j], number_of_channels[i]);
              j++;
          }
          i++;
      }
      if (index > 0) str[index - 1] = 0;
      printf("rates_s = %s \n", str);

      FILE* fp;
      fp = fopen("Output_f.csv", "w");
      fprintf(fp, "%s,%s,%s\n",
          "Seed",
          "Arrival Rate",
          str

      );


      i = 0;
      j = 0;
      index = 0;
      l = 0;
      h = 0;
      while ((RANDOM_SEEDS[l]) != 0) {
          i = 0;
          j = 0;
          while (arrival_rates[i] != -1) {
              j = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[i] * MEAN_CALL_DURATION);
              while (number_of_channels[j] != -1) {
                  k = 0;
                  while (hangup_time[k] != -1) {
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", mean_queue_time[i + (j * 500) + (k * 500 * 100)]);
                      k++;
                  }
                  j++;
              }

              if (index > 0) str[--index] = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n");
              i++;
          }
          l++;

      }

      i = 0;
      j = 0;
      //index = 0;
      l = 0;
      h = 0;
      while ((RANDOM_SEEDS[l]) != 0) {
          i = 0;
          j = 0;
          while (arrival_rates[i] != -1) {
              j = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[i] * MEAN_CALL_DURATION);
              while (number_of_channels[j] != -1) {
                  k = 0;
                  while (hangup_time[k] != -1) {
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", mean_queue_time_no_hangup[i + (j * 500) + (k * 500 * 100)]);
                      k++;
                  }
                  j++;
              }

              if (index > 0) str[--index] = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n");
              i++;
          }
          l++;

      }

      i = 0;
      j = 0;
      //index = 0;
      l = 0;
      h = 0;
      while ((RANDOM_SEEDS[l]) != 0) {
          i = 0;
          j = 0;
          while (arrival_rates[i] != -1) {
              j = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%d,", RANDOM_SEEDS[l]);
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", arrival_rates[i] * MEAN_CALL_DURATION);
              while (number_of_channels[j] != -1) {
                  k = 0;
                  while (hangup_time[k] != -1) {
                      index += snprintf(&str[index], OUT_STR_SIZE - index, "%.10f,", pb_hangup[i + (j * 500) + (k * 500 * 100)]);
                      k++;
                  }
                  j++;
              }

              if (index > 0) str[--index] = 0;
              index += snprintf(&str[index], OUT_STR_SIZE - index, "%s", "\n");
              i++;
          }
          l++;

      }



      fprintf(fp, "%s\n",


          str
      );
  }

  xfree(pb_hangup);
  xfree(mean_queue_time);
  xfree(mean_queue_time_no_hangup);


  /* Pause before finishing. */
  getchar();
  return 0;
}













