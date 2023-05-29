
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
schedule_end_packet_transmission_event(Simulation_Run_Ptr simulation_run,
				       double event_time,
				       Server_Ptr link)
{
  Event event;

  event.description = "Packet Xmt End";
  event.function = end_packet_transmission_event;
  event.attachment = (void *) link;

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
end_packet_transmission_event(Simulation_Run_Ptr simulation_run, void * switch_data)
{
  Simulation_Run_Data_Ptr data;
  Packet_Ptr this_packet, next_packet;

  TRACE(printf("End Of Packet.\n"););

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);

  /* 
   * Packet transmission is finished. Take the packet off the data link.
   */

  this_packet = (Packet_Ptr) server_get(((Switch_Ptr)switch_data)->link);

  double packet_delay = simulation_run_get_time(simulation_run) - this_packet->arrive_time;
  ((Switch_Ptr)switch_data)->accumulated_delay += packet_delay;

  if (((Switch_Ptr)switch_data)->previous_departure_time != 0) {
	  ((Switch_Ptr)switch_data)->accumulated_departure_intervals += simulation_run_get_time(simulation_run) - ((Switch_Ptr)switch_data)->previous_departure_time;
	  ((Switch_Ptr)switch_data)->departure_count++;
  }
  ((Switch_Ptr)switch_data)->previous_departure_time = simulation_run_get_time(simulation_run);

  if (((Switch_Ptr)switch_data)->id_ == 1) {
	  //apply p12
	  double prob = (double)rand() / (double)(RAND_MAX);
	  if (prob < data->p12) {
		//goes to link2
		  //arrival stats
		  if (data->switch2->previous_arrival_time != 0) {
			  data->switch2->accumulated_arrival_intervals += simulation_run_get_time(simulation_run) - data->switch2->previous_arrival_time;
			  data->switch2->arrival_count++;
		  }
		  data->switch2->previous_arrival_time = simulation_run_get_time(simulation_run);
		  //configure packet
		  this_packet->service_time = get_packet_transmission_time(data->switch2);
		  this_packet->status = WAITING;
		  this_packet->source_id = ((Switch_Ptr)switch_data)->id_;
		  this_packet->destination_id = data->switch2->id_;
		  //send
		  if (server_state(data->switch2->link) == BUSY) {
			  fifoqueue_put(data->switch2->buffer_data, (void*)this_packet);
		  }
		  else {
			  start_transmission_on_link(simulation_run, this_packet, data->switch2);
		  }
	  }
	  else {
		  //goes to link3
		  //arrival stats
		  if (data->switch3->previous_arrival_time != 0) {
			  data->switch3->accumulated_arrival_intervals += simulation_run_get_time(simulation_run) - data->switch3->previous_arrival_time;
			  data->switch3->arrival_count++;
		  }
		  data->switch3->previous_arrival_time = simulation_run_get_time(simulation_run);
		  //configure packet
		  this_packet->service_time = get_packet_transmission_time(data->switch3);
		  this_packet->status = WAITING;
		  this_packet->source_id = ((Switch_Ptr)switch_data)->id_;
		  this_packet->destination_id = data->switch3->id_;
		  //send
		  if (server_state(data->switch3->link) == BUSY) {
			  fifoqueue_put(data->switch3->buffer_data, (void*)this_packet);
		  }
		  else {
			  start_transmission_on_link(simulation_run, this_packet, data->switch3);
		  }
	  }
  }
  else {
	  /* Collect statistics. */
	  data->number_of_packets_processed++;
	  /*if (data->number_of_packets_processed == RUNLENGTH) {
		  output_progress_msg_to_screen(simulation_run);
	  }*/
	  
	  data->accumulated_delay += packet_delay;

	  if(this_packet->type == VOICE){
		  ((Switch_Ptr)switch_data)->number_of_packets_processed_voice++;
		  ((Switch_Ptr)switch_data)->accumulated_delay_voice += packet_delay;
	  }
	  else if (this_packet->type == DATA) {
		  ((Switch_Ptr)switch_data)->number_of_packets_processed_data++;
		  ((Switch_Ptr)switch_data)->accumulated_delay_data += packet_delay;
	  }

	  if (this_packet->arrival_id == 1) {
		  data->number_of_packets_processed_link1++;
		  data->accumulated_delay_link1 += packet_delay;
	  }
	  else if (this_packet->arrival_id == 2) {
		  data->number_of_packets_processed_link2++;
		  data->accumulated_delay_link2 += packet_delay;
	  }
	  else if (this_packet->arrival_id == 3) {
		  data->number_of_packets_processed_link3++;
		  data->accumulated_delay_link3 += packet_delay;
	  }
	  

	  if (packet_delay > 1e-3 * 20) data->delay_exceeds_20p_counter++;

	  /* Output activity blip every so often. */
	  output_progress_msg_to_screen(simulation_run);

	  /* This packet is done ... give the memory back. */
	  xfree((void*)this_packet);
  }

  /* 
   * See if there is are packets waiting in the buffer. If so, take the next one
   * out and transmit it immediately.
  */

  if(fifoqueue_size(((Switch_Ptr)switch_data)->buffer_voice) > 0) {
    next_packet = (Packet_Ptr) fifoqueue_get(((Switch_Ptr)switch_data)->buffer_voice);
    start_transmission_on_link(simulation_run, next_packet, switch_data);
  } else if (fifoqueue_size(((Switch_Ptr)switch_data)->buffer_data) > 0) {
	  next_packet = (Packet_Ptr)fifoqueue_get(((Switch_Ptr)switch_data)->buffer_data);
	  start_transmission_on_link(simulation_run, next_packet, switch_data);
  }
}

/*
 * This function ititiates the transmission of the packet passed to the
 * function. This is done by placing the packet in the server. The packet
 * transmission end event for this packet is then scheduled.
 */

void
start_transmission_on_link(Simulation_Run_Ptr simulation_run, 
			   Packet_Ptr this_packet,
	Switch_Ptr switch_data)
{
  TRACE(printf("Start Of Packet.\n");)

  server_put(((Switch_Ptr)switch_data)->link, (void*) this_packet);
  this_packet->status = XMTTING;

  /* Schedule the end of packet transmission event. */
  schedule_end_packet_transmission_event(simulation_run,
	 simulation_run_get_time(simulation_run) + this_packet->service_time,
	 (void *)switch_data);
}

/*
 * Get a packet transmission time. For now it is a fixed value defined in
 * simparameters.h
 */

double
get_packet_transmission_time(void* switch_data)
{
  return ((Switch_Ptr)switch_data)->xmt_time;
}


