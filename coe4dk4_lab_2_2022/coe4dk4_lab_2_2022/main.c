
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
 * Switch functions.
 *
 * Create and return a pointer to an empty Switch.
 */
Switch_Ptr
switch_new(void)
{
    Switch_Ptr switch_ptr;

    switch_ptr = (Switch_Ptr)xmalloc(sizeof(Switch));
    switch_ptr->buffer_data = NULL;
    switch_ptr->buffer_voice = NULL;
    switch_ptr->link = NULL; 
    switch_ptr->arrival_rate = 0;
    switch_ptr->id_ = 0;
    switch_ptr->accumulated_departure_intervals = 0;
    switch_ptr->departure_count = 0;
    switch_ptr->previous_departure_time = 0;
    switch_ptr->accumulated_arrival_intervals = 0;
    switch_ptr->arrival_count = 0;
    switch_ptr->previous_arrival_time = 0;
    switch_ptr->accumulated_delay = 0;
    switch_ptr->accumulated_delay_voice = 0;
    switch_ptr->accumulated_delay_data = 0;
    switch_ptr->number_of_packets_processed_voice = 0;
    switch_ptr->number_of_packets_processed_data = 0;
    return switch_ptr;
}
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
  //calc arrival rates
  double arrival_rates[500];
  double arrival_rate_min = 20;
  int arrival_rate_data_points = 101;
  double arrival_rate_max = 26;
  for (int i = 0; i < arrival_rate_data_points; i++) {
      if (i == 0) arrival_rates[i] = arrival_rate_min;
      else arrival_rates[i] = arrival_rates[i - 1] + (double)((double)(arrival_rate_max - arrival_rate_min)/(arrival_rate_data_points-1));
  }
  arrival_rates[arrival_rate_data_points] = -1;
  
  //calc p12 vals
  double p12_vals[500];
  double p12_min = 0.5;
  int p12_data_points = 1;
  double p12_max = 0.52;
  for (int i = 0; i < p12_data_points; i++) {
      if (i == 0) p12_vals[i] = p12_min;
      else p12_vals[i] = p12_vals[i - 1] + (double)((double)(p12_max - p12_min) / (p12_data_points-1));
  }
  p12_vals[p12_data_points] = -1;

  int k = 0;
  
  data.writes_to_file = 0;
  


  

  /* 
   * Loop for each random number generator seed, doing a separate
   * simulation_run run for each.
   */
  while (p12_vals[k++] != -1) {
      int j = 0;
      while ((random_seed = RANDOM_SEEDS[j++]) != 0) {

          simulation_run = simulation_run_new(); /* Create a new simulation run. */

          /*
           * Set the simulation_run data pointer to our data object.
           */

          simulation_run_attach_data(simulation_run, (void*)&data);

          /*
           * Initialize the simulation_run data variables, declared in main.h.
           */

          data.blip_counter = 0;
          data.arrival_count = 0;
          data.arrival_rate = arrival_rates[k-1];
          data.number_of_packets_processed = 0;
          data.delay_exceeds_20p_counter = 0;
          data.accumulated_delay = 0.0;
          data.random_seed = random_seed;
          data.p12 = p12_vals[k - 1];

          data.accumulated_delay_link1 = 0.0;
          data.accumulated_delay_link2 = 0.0;
          data.accumulated_delay_link3 = 0.0;

          data.number_of_packets_processed_link1 = 0.0;
          data.number_of_packets_processed_link2 = 0.0;
          data.number_of_packets_processed_link3 = 0.0;

          /*
           * Create the packet buffer and transmission link, declared in main.h.
           */

          /*data.buffer = fifoqueue_new();
          data.link1 = server_new();
          data.link2 = server_new();*/
          
          //create switches
          data.switch1 = switch_new();
          data.switch2 = switch_new();
          data.switch3 = switch_new();
          //switch data
          data.switch1->id_ = 1;
          data.switch2->id_ = 2;
          data.switch3->id_ = 3;
          data.switch1->arrival_rate = 750;
          data.switch2->arrival_rate = 500;
          data.switch3->arrival_rate = 500;
          data.switch1->xmt_time = ((double)PACKET_LENGTH / 2e6);
          data.switch2->xmt_time = ((double)PACKET_LENGTH / 1e6);
          data.switch3->xmt_time = ((double)PACKET_LENGTH / 1e6);
          data.switch1->buffer_data = fifoqueue_new();
          data.switch1->buffer_voice = fifoqueue_new();
          data.switch2->buffer_data = fifoqueue_new();
          data.switch2->buffer_voice = fifoqueue_new();
          data.switch3->buffer_data = fifoqueue_new();
          data.switch3->buffer_voice = fifoqueue_new();

          data.switch1->link = server_new();
          data.switch2->link = server_new();
          data.switch3->link = server_new();
          
          



          /*
           * Set the random number generator seed for this run.
           */

          random_generator_initialize(random_seed);

          /*
           * Schedule the initial packet arrival for the current clock time (= 0).
           */

          /*schedule_packet_arrival_event_voice(simulation_run,
              simulation_run_get_time(simulation_run), data.switch1);

          schedule_packet_arrival_event_data(simulation_run,
              simulation_run_get_time(simulation_run), data.switch1);*/

          schedule_packet_arrival_event(simulation_run,
              simulation_run_get_time(simulation_run), data.switch1);
          
          schedule_packet_arrival_event(simulation_run,
              simulation_run_get_time(simulation_run), data.switch2);

          schedule_packet_arrival_event(simulation_run,
              simulation_run_get_time(simulation_run), data.switch3);

          /*
           * Execute events until we are finished.
           */

          while (data.number_of_packets_processed < RUNLENGTH) {
              simulation_run_execute_event(simulation_run);
          }

          /*
           * Output results and clean up after ourselves.
           */

          output_results(simulation_run);
          cleanup_memory(simulation_run);
      }
  }

  getchar();   /* Pause before finishing. */
  return 0;
}












