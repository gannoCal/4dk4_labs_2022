
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
#include "trace.h"
#include "main.h"
#include "output.h"
#include "packet_transmission.h"

/******************************************************************************/

/*
 * This function will schedule the end of a packet transmission at a time given
 * by event_time. At that time the function "end_packet_transmission" (defined
 * in packet_transmissionl.c) is executed. A packet object is attached to the
 * event and is recovered in end_packet_transmission.c.
 */

long
schedule_end_packet_transmission_to_base_event(Simulation_Run_Ptr simulation_run,
				       double event_time,
				       Server_Ptr link)
{
  Event event;

  event.description = "Packet Xmt End";
  event.function = end_packet_transmission_to_base_event;
  event.attachment = (void *) link;

  return simulation_run_schedule_event(simulation_run, event, event_time);
}



long
schedule_end_packet_job_on_cloud_event(Simulation_Run_Ptr simulation_run,
	double event_time,
	Server_Ptr link)
{
	Event event;

	event.description = "Packet Xmt End Cloud";
	event.function = end_packet_job_on_cloud_event;
	event.attachment = (void*)link;

	return simulation_run_schedule_event(simulation_run, event, event_time);
}

/******************************************************************************/

/*
 * This is the event function which is executed when the end of a packet
 * transmission event occurs. It updates its collected data then checks to see
 * if there are other packets waiting in the fifo queue. If that is the case it
 * starts the transmission of the next packet.
 */

void
end_packet_transmission_to_base_event(Simulation_Run_Ptr simulation_run, void * link)
{
  Simulation_Run_Data_Ptr data;
  Packet_Ptr this_packet, next_packet;

  TRACE(printf("End Of Packet.\n"););

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  /* 
   * Packet transmission is finished. Take the packet off the data link.
   */

  this_packet = (Packet_Ptr) server_get(link);

  /* Collect statistics. */
  data->number_of_packets_processed_base++;
  

  /* Output activity blip every so often. */
  output_progress_msg_to_screen(simulation_run);

  int station_id = this_packet->source_id;

  /* This packet is done ... give the memory back. */
  //xfree((void *) this_packet);

  /* 
   * See if there is are packets waiting in the buffer. If so, take the next one
   * out and transmit it immediately.
  */
  if (station_id == 1) {
	  if (fifoqueue_size(data->good_phone_buffer) > 0) {
		  next_packet = (Packet_Ptr)fifoqueue_get(data->good_phone_buffer);
		  start_transmission_to_base(simulation_run, next_packet, data->good_phone_server);
	  }
  }
  else {
	  if (fifoqueue_size(data->bad_phone_buffer) > 0) {
		  next_packet = (Packet_Ptr)fifoqueue_get(data->bad_phone_buffer);
		  start_transmission_to_base(simulation_run, next_packet, data->bad_phone_server);
	  }
  }

  // base to cloud transmission time ignored - very small
  //schedule new cloud server job
  this_packet->status = WAITING;
  this_packet->service_time = 0.5;
  data->priority_flag = data->accumulated_delay_good / data->number_of_packets_processed_good > data->accumulated_delay_bad / data->number_of_packets_processed_bad ? 1 : 0;
  if (server_state(data->cloud_server) == BUSY) {
	  if (((Packet_Ptr)data->cloud_server->customer_in_service)->source_id != data->priority_flag && this_packet->source_id == data->priority_flag) {
		  Packet_Ptr running_packet = (Packet_Ptr)server_get(data->cloud_server);
		  simulation_run_deschedule_event(simulation_run, running_packet->job_id);
		  //Packet_Ptr running_packet = (Packet_Ptr)data->cloud_server->customer_in_service;
		  running_packet->service_time = simulation_run_get_time(simulation_run) - running_packet->service_start_time;
		  if (running_packet->source_id == 1) {
			  fifoqueue_put(data->cloud_server_good_buffer, (void*)running_packet);
		  }
		  else {
			  fifoqueue_put(data->cloud_server_bad_buffer, (void*)running_packet);
		  }
		  start_job_on_cloud(simulation_run, this_packet, data->cloud_server);
	  }
	  else {

		  if (station_id == 1) {
			  fifoqueue_put(data->cloud_server_good_buffer, (void*)this_packet);
		  }
		  else {
			  fifoqueue_put(data->cloud_server_bad_buffer, (void*)this_packet);
		  }
	  }
  }
  else {
	  /*if (data->priority_flag == 0) {
		  if (station_id == 1 && fifoqueue_size(data->cloud_server_bad_buffer) > 0) {
			  fifoqueue_put(data->cloud_server_good_buffer, (void*)this_packet);
			  next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_bad_buffer);
			  start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		  }
		  else {


			  start_job_on_cloud(simulation_run, this_packet, data->cloud_server);
		  }
	  }
	  else {
		  if (station_id == 0 && fifoqueue_size(data->cloud_server_good_buffer) > 0) {
			  fifoqueue_put(data->cloud_server_bad_buffer, (void*)this_packet);
			  next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_good_buffer);
			  start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		  }
		  else {


			  start_job_on_cloud(simulation_run, this_packet, data->cloud_server);
		  }
	  }*/
	  start_job_on_cloud(simulation_run, this_packet, data->cloud_server);
	  
  }

  if (data->max_bad_buffer < fifoqueue_size(data->cloud_server_bad_buffer)) data->max_bad_buffer = fifoqueue_size(data->cloud_server_bad_buffer);
  if (data->max_good_buffer < fifoqueue_size(data->cloud_server_good_buffer)) data->max_good_buffer = fifoqueue_size(data->cloud_server_good_buffer);

}


void
end_packet_job_on_cloud_event(Simulation_Run_Ptr simulation_run, void* link)
{
	Simulation_Run_Data_Ptr data;
	Packet_Ptr this_packet, next_packet;

	TRACE(printf("End Of Packet.\n"););

	data = (Simulation_Run_Data_Ptr)simulation_run_data(simulation_run);

	/*
	 * Packet transmission is finished. Take the packet off the data link.
	 */

	this_packet = (Packet_Ptr)server_get(link);

	/* Collect statistics. */
	data->number_of_packets_processed++;

	int station_id = this_packet->source_id;
	if (station_id == 1) {
		data->accumulated_delay_good += simulation_run_get_time(simulation_run) -
			this_packet->arrive_time;
		data->number_of_packets_processed_good++;
		data->priority_count++;
	}
	else {
		data->accumulated_delay_bad += simulation_run_get_time(simulation_run) -
			this_packet->arrive_time;
		data->number_of_packets_processed_bad++;
		data->priority_count--;
	}

	//data->priority_flag = data->priority_count == 0 ? data->priority_flag : data->priority_count > 0 ? 0 : 1;
	//data->priority_flag = data->accumulated_delay_good / data->number_of_packets_processed_good > data->accumulated_delay_bad / data->number_of_packets_processed_bad ? 1 : 0;
	

	/* Output activity blip every so often. */
	output_progress_msg_to_screen(simulation_run);

	

	/* This packet is done ... give the memory back. */
	xfree((void *) this_packet);

	/*
	 * See if there is are packets waiting in the buffer. If so, take the next one
	 * out and transmit it immediately.
	*/
	

	// base to cloud transmission time ignored - very small
	//schedule new cloud server job
	/*if (data->priority_flag == 0) {
		if (fifoqueue_size(data->cloud_server_bad_buffer) > 0) {
			next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_bad_buffer);
			start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		}
		else if (fifoqueue_size(data->cloud_server_good_buffer) > 0) {
			next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_good_buffer);
			start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		}
	}
	else {
		if (fifoqueue_size(data->cloud_server_good_buffer) > 0) {
			next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_good_buffer);
			start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		}
		else if (fifoqueue_size(data->cloud_server_bad_buffer) > 0) {
			next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_bad_buffer);
			start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
		}
	}*/
	if (fifoqueue_size(data->cloud_server_bad_buffer) > 0 && fifoqueue_size(data->cloud_server_good_buffer) > 0) {
		Packet_Ptr good_packet = fifoqueue_see_front(data->cloud_server_good_buffer);
		Packet_Ptr bad_packet = fifoqueue_see_front(data->cloud_server_bad_buffer);
		next_packet = data->accumulated_delay_good/data->number_of_packets_processed_good < data->accumulated_delay_bad/ data->number_of_packets_processed_bad ? (Packet_Ptr)fifoqueue_get(data->cloud_server_bad_buffer) : (Packet_Ptr)fifoqueue_get(data->cloud_server_good_buffer);
		start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
	}
	else if (fifoqueue_size(data->cloud_server_good_buffer) > 0) {
		next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_good_buffer);
		start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
	}
	else if (fifoqueue_size(data->cloud_server_bad_buffer) > 0) {
		next_packet = (Packet_Ptr)fifoqueue_get(data->cloud_server_bad_buffer);
		start_job_on_cloud(simulation_run, next_packet, data->cloud_server);
	}

}

/*
 * This function ititiates the transmission of the packet passed to the
 * function. This is done by placing the packet in the server. The packet
 * transmission end event for this packet is then scheduled.
 */

void
start_transmission_to_base(Simulation_Run_Ptr simulation_run, 
			   Packet_Ptr this_packet,
			   Server_Ptr link)
{
  TRACE(printf("Start Of Packet.\n");)

  server_put(link, (void*) this_packet);
  this_packet->status = XMTTING;

  /* Schedule the end of packet transmission event. */
  schedule_end_packet_transmission_to_base_event(simulation_run,
	 simulation_run_get_time(simulation_run) + this_packet->service_time,
	 (void *) link);
}


void
start_job_on_cloud(Simulation_Run_Ptr simulation_run,
	Packet_Ptr this_packet,
	Server_Ptr link)
{
	TRACE(printf("Start Of Packet.\n");)

		server_put(link, (void*)this_packet);
	this_packet->status = XMTTING;
	this_packet->service_start_time = simulation_run_get_time(simulation_run);
	/* Schedule the end of packet transmission event. */
	this_packet->job_id = schedule_end_packet_job_on_cloud_event(simulation_run,
		simulation_run_get_time(simulation_run) + this_packet->service_time,
		(void*)link);
}

/*
 * Get a packet transmission time. For now it is a fixed value defined in
 * simparameters.h
 */

double
get_packet_transmission_time(void)
{
  return ((double) PACKET_XMT_TIME);
}


