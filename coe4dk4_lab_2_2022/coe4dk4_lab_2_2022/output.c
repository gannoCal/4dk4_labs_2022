
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

/******************************************************************************/

#include <stdio.h>
#include "simparameters.h"
#include "main.h"
#include "output.h"

/******************************************************************************/

/*
 * This function outputs a progress message to the screen to indicate this are
 * working.
 */

void
output_progress_msg_to_screen(Simulation_Run_Ptr simulation_run)
{
  double percentage_done;
  Simulation_Run_Data_Ptr data;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  data->blip_counter++;

  if((data->blip_counter >= BLIPRATE)
     ||
     (data->number_of_packets_processed >= RUNLENGTH)) {

    data->blip_counter = 0;

    percentage_done =
      100 * (double) data->number_of_packets_processed/RUNLENGTH;

    printf("%3.0f%% ", percentage_done);

    printf("Successfully Xmtted Pkts  = %ld (Arrived Pkts = %ld) \r", 
	   data->number_of_packets_processed, data->arrival_count);

    fflush(stdout);
  }

}

/*
 * When a simulation_run run is completed, this function outputs various
 * collected statistics on the screen.
 */

void
output_results(Simulation_Run_Ptr simulation_run)
{
  double xmtted_fraction;
  Simulation_Run_Data_Ptr data;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  printf("\n");
  printf("Random Seed = %d \n", data->random_seed);
  printf("Packet arrival count = %ld \n", data->arrival_count);

  xmtted_fraction = (double) data->number_of_packets_processed /
    data->arrival_count;

  printf("Transmitted packet count  = %ld (Service Fraction = %.5f)\n",
	 data->number_of_packets_processed, xmtted_fraction);

  printf("Transmitted packet count[data]  = %ld \n",
      data->switch1->number_of_packets_processed_data);

  printf("Transmitted packet count[voice]  = %ld\n",
      data->switch1->number_of_packets_processed_voice);

  printf("Arrival rate = %.3f packets/second \n", (double)data->arrival_rate);

  printf("p12 = %.3f  \n", (double)data->p12);

  
  printf("switch 1 arrival rate = %.3f  \n", (double)data->switch1->arrival_count / data->switch1->accumulated_arrival_intervals);
  printf("switch 1 departure rate = %.3f  \n", (double)data->switch1->departure_count / data->switch1->accumulated_departure_intervals);
  printf("switch 1 mean delay = %.3f  \n", 1e3 * (double)data->switch1->accumulated_delay / data->switch1->departure_count);
  printf("switch 1 [data] mean delay = %.3f  \n", 1e3 * (double)data->switch1->accumulated_delay_data / data->switch1->number_of_packets_processed_data);
  printf("switch 1 [voice] mean delay = %.3f  \n", 1e3 * (double)data->switch1->accumulated_delay_voice / data->switch1->number_of_packets_processed_voice);
  printf("switch 3 arrival rate = %.3f  \n", (double)data->switch3->arrival_count / data->switch3->accumulated_arrival_intervals);
  printf("switch 3 departure rate = %.3f  \n", (double)data->switch3->departure_count / data->switch3->accumulated_departure_intervals);
  printf("switch 3 mean delay = %.3f  \n", 1e3 * (double)data->switch3->accumulated_delay / data->switch3->departure_count);

  printf("Mean Delay (msec) = %.2f \n",
	 1e3*data->accumulated_delay/data->number_of_packets_processed);

  printf("final clock %.2f \n",
      simulation_run->clock->time
);

  printf("\n");

  FILE* fp;
  if (data->writes_to_file == 0) { 
      fp = fopen("Output.csv", "w"); 
      fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
          "data->arrival_rate",
          "1e3 * data->accumulated_delay / data->number_of_packets_processed",
          "data->random_seed",
          "data->delay_exceeds_20p_counter",
          "data->number_of_packets_processed",
          "(double)data->delay_exceeds_20p_counter / data->number_of_packets_processed",
          "(double)data->p12",
          "1e3 * (double)data->switch1->accumulated_delay_data / data->switch1->number_of_packets_processed_data",
          "1e3 * (double)data->switch1->accumulated_delay_voice / data->switch1->number_of_packets_processed_voice",
          "(double)data->switch1->accumulated_delay_data",
          "data->switch1->number_of_packets_processed_data",
          "(double)data->switch1->accumulated_delay_voice",
          "data->switch1->number_of_packets_processed_voice",
          "p12",
          "mean delay link 1",
          "mean delay link 2",
          "mean delay link 3"
      );
  }
  else fp = fopen("Output.csv", "a");
  fprintf(fp, "%.7f,%.2f,%d,%d,%d,%.7f,%.7f,%.7f,%.7f,%.7f,%d,%.7f,%d,%.7f,%.2f,%.2f,%.2f\n", 
      data->arrival_rate, 
      1e3*data->accumulated_delay/data->number_of_packets_processed, 
      data->random_seed, 
      data->delay_exceeds_20p_counter, 
      data->number_of_packets_processed, 
      (double)data->delay_exceeds_20p_counter/ data->number_of_packets_processed, 
      (double)data->p12, 
      1e3 * (double)data->switch1->accumulated_delay_data / data->switch1->number_of_packets_processed_data, 
      1e3 * (double)data->switch1->accumulated_delay_voice / data->switch1->number_of_packets_processed_voice, 
      (double)data->switch1->accumulated_delay_data, 
      data->switch1->number_of_packets_processed_data, 
      (double)data->switch1->accumulated_delay_voice, 
      data->switch1->number_of_packets_processed_voice,
      (double)data->p12,
      1e3 * data->accumulated_delay_link1 / data->number_of_packets_processed_link1,
      1e3 * data->accumulated_delay_link2 / data->number_of_packets_processed_link2,
      1e3 * data->accumulated_delay_link3 / data->number_of_packets_processed_link3
  );
  fflush(fp);
  data->writes_to_file++;

}



