
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

#ifndef _MAIN_H_
#define _MAIN_H_

/******************************************************************************/

#include "simlib.h"
#include "simparameters.h"

/******************************************************************************/

typedef struct _simulation_run_data_ 
{
  Fifoqueue_Ptr cloud_server_good_buffer;
  Fifoqueue_Ptr cloud_server_bad_buffer;
  Fifoqueue_Ptr good_phone_buffer;
  Fifoqueue_Ptr bad_phone_buffer;
  Server_Ptr cloud_server;
  Server_Ptr good_phone_server;
  Server_Ptr bad_phone_server;
  long int blip_counter;
  long int arrival_count;
  long int number_of_packets_processed;
  long int number_of_packets_processed_base;
  long int number_of_packets_processed_good;
  long int number_of_packets_processed_bad;
  double accumulated_delay_good;
  double accumulated_delay_bad;
  unsigned random_seed;
  long int runs_required;
  long int run_count;
  double arrival_rate;
  long int max_bad_buffer;
  long int max_good_buffer;
  int priority_flag;
  int priority_count;
} Simulation_Run_Data, * Simulation_Run_Data_Ptr;

typedef enum {XMTTING, WAITING} Packet_Status;

typedef struct _packet_ 
{
  double arrive_time;
  double service_time;
  double service_start_time;
  int source_id;
  int destination_id;
  long int job_id;
  Packet_Status status;
} Packet, * Packet_Ptr;

/*
 * Function prototypes
 */

int
main(void);

/******************************************************************************/

#endif /* main.h */


