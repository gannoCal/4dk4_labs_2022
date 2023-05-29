
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
typedef struct _switch_
{
	Fifoqueue_Ptr buffer_voice;
	Fifoqueue_Ptr buffer_data;
	Server_Ptr link;
	double arrival_rate;
	double xmt_time;
	int id_;
	//departure stats
	double accumulated_departure_intervals;
	long int departure_count;
	double previous_departure_time;
	//arrival stats
	double accumulated_arrival_intervals;
	long int arrival_count;
	double previous_arrival_time;
	//delay
	double accumulated_delay;
	double accumulated_delay_voice;
	double accumulated_delay_data;
	long int number_of_packets_processed_voice;
	long int number_of_packets_processed_data;
} Switch, * Switch_Ptr;

typedef struct _simulation_run_data_ 
{
	Switch_Ptr switch1;
	Switch_Ptr switch2;
	Switch_Ptr switch3;
  long int blip_counter;
  long int arrival_count;
  double arrival_rate;
  double p12;
  long int writes_to_file;
  long int delay_exceeds_20p_counter;
  long int number_of_packets_processed;
  double accumulated_delay;
  long int number_of_packets_processed_link1;
  double accumulated_delay_link1;
  long int number_of_packets_processed_link2;
  double accumulated_delay_link2;
  long int number_of_packets_processed_link3;
  double accumulated_delay_link3;
  unsigned random_seed;
} Simulation_Run_Data, * Simulation_Run_Data_Ptr;

typedef enum {XMTTING, WAITING} Packet_Status;
typedef enum {NONE ,DATA, VOICE } Packet_Type;

typedef struct _packet_ 
{
  double arrive_time;
  double service_time;
  int source_id;
  int destination_id;
  int arrival_id;
  Packet_Status status;
  Packet_Type type;
} Packet, * Packet_Ptr;



/*
 * Function prototypes
 */

int
main(void);

/******************************************************************************/

#endif /* main.h */


