
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

#include <stdio.h>
#include "simparameters.h"
#include "main.h"
#include "output.h"


double fact(double x)
{
    return tgamma(x + 1.);
}

/*******************************************************************************/

void output_progress_msg_to_screen(Simulation_Run_Ptr this_simulation_run)
{
  double percentagedone;
  Simulation_Run_Data_Ptr sim_data;

  sim_data = (Simulation_Run_Data_Ptr) simulation_run_data(this_simulation_run);

  sim_data->blip_counter++;

  if((sim_data->blip_counter >= BLIPRATE)
     ||
     (sim_data->number_of_calls_processed >= RUNLENGTH))
    {
      sim_data->blip_counter = 0;

      percentagedone =
	100 * (double) sim_data->number_of_calls_processed/RUNLENGTH;

      printf("%3.0f%% ", percentagedone);

      printf("Call Count = %ld \r", sim_data->number_of_calls_processed);

      fflush(stdout);
    }
}

/*******************************************************************************/

void output_results(Simulation_Run_Ptr this_simulation_run)
{
  double xmtted_fraction;
  Simulation_Run_Data_Ptr sim_data;

  sim_data = (Simulation_Run_Data_Ptr) simulation_run_data(this_simulation_run);

  printf("\n");

  printf("random seed = %d \n", sim_data->random_seed);
  printf("call arrival count = %ld \n", sim_data->call_arrival_count);
  printf("blocked call count = %ld \n", sim_data->blocked_call_count);

  printf("trunks = %d \n", sim_data->trunk_count);
  printf("arrival rate = %.5f \n", sim_data->arrival_rate); 
  
  printf("mean duration = %.5f \n", (double)MEAN_CALL_DURATION);
  double offered_load = MEAN_CALL_DURATION * sim_data->arrival_rate;
  printf("Offered Load (A) = %.5f \n", offered_load);

  //theoretical_erlang_B
  double accum = 0;
  for (int i = 0; i <= sim_data->trunk_count; i++) {
      accum += (double)pow(offered_load, i) / fact(i);
  }
  double theoretical_erlang_B = ((double)pow(offered_load, sim_data->trunk_count) / fact(sim_data->trunk_count)) / accum;
  printf("theoretical_erlang_B = %.5f \n", theoretical_erlang_B);

  xmtted_fraction = (double) (sim_data->call_arrival_count -
      sim_data->blocked_call_count)/sim_data->call_arrival_count;

  printf("Blocking probability = %.5f (Service fraction = %.5f)\n",
	 1-xmtted_fraction, xmtted_fraction);

  printf("\n");

  //char rates_s[1000][30];
  //int n = 0;
  //while (sim_data->arrival_rates[n] != -1) {
  //    printf("arrival rate = %.5f \n", sim_data->arrival_rates[n]);
  //    char arate[30];
  //    sprintf(arate, "%.5f", sim_data->arrival_rates[n]);
  //    for (int i = 0; i < 30; i++) {
  //        rates_s[2 * n][i] = arate[i];
  //    }
  //    
  //    rates_s[2 * n + 1][0] = ',';
  //    n++;
  //}

  //char str[1000];
  //int i = 0;
  //int index = 0;
  //while (sim_data->arrival_rates[i] != -1) {
  //    index += snprintf(&str[index], 1000 - index, ",Rate=%.02f", sim_data->arrival_rates[i]);
  //    i++;
  //}
  ////if(index > 0) str[index - 1] = 0;
  //printf("rates_s = %s \n", str);

  /*FILE* fp;
  if (sim_data->writes_to_file == 0) {
      fp = fopen("Output.csv", "w");
      fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
          "random seed",
          "call arrival count",
          "blocked call count",
          "Blocking probability",
          "Service fraction",
          "Offered Load",
          "trunks",
          "theoretical_erlang_B",
          ",,,,,",
          "trunks",
          "Blocking probability",
          "Offered Load"
      );
  }
  else fp = fopen("Output.csv", "a");
  fprintf(fp, "%d,%ld,%ld,%.10f,%.5f,%.10f,%d,%.10f,%s,%d,%.10f,%.10f\n",
      sim_data->random_seed,
      sim_data->call_arrival_count,
      sim_data->blocked_call_count,
      1 - xmtted_fraction,
      xmtted_fraction,
      offered_load,
      sim_data->trunk_count,
      theoretical_erlang_B,
      ",,,,,",
      sim_data->trunk_count,
      1 - xmtted_fraction,
      offered_load
  );
  fflush(fp);*/
  sim_data->writes_to_file++;
}





