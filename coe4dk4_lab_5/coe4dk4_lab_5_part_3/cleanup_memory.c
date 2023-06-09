
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

#include "simlib.h"
#include "main.h"
#include "cleanup_memory.h"

/******************************************************************************/

/*
 * When a simulation_run run is finished, this function cleans up the memory
 * that has been allocated.
 */

void
cleanup_memory (Simulation_Run_Ptr simulation_run)
{
  Simulation_Run_Data_Ptr data;
  Fifoqueue_Ptr buffer;
  Server_Ptr link;

  data = (Simulation_Run_Data_Ptr) simulation_run_data(simulation_run);
  buffer = data->good_phone_buffer;
  link = data->good_phone_server;

  if(link->state == BUSY) /* Clean out the server. */
    xfree(server_get(link));
  xfree(link);

  while (fifoqueue_size(buffer) > 0) /* Clean out the queue. */
    xfree(fifoqueue_get(buffer));
  xfree(buffer);

  buffer = data->bad_phone_buffer;
  link = data->bad_phone_server;

  if (link->state == BUSY) /* Clean out the server. */
      xfree(server_get(link));
  xfree(link);

  while (fifoqueue_size(buffer) > 0) /* Clean out the queue. */
      xfree(fifoqueue_get(buffer));
  xfree(buffer);

  buffer = data->cloud_server_buffer;
  link = data->cloud_server;

  if (link->state == BUSY) /* Clean out the server. */
      xfree(server_get(link));
  xfree(link);

  while (fifoqueue_size(buffer) > 0) /* Clean out the queue. */
      xfree(fifoqueue_get(buffer));
  xfree(buffer);

  simulation_run_free_memory(simulation_run); /* Clean up the simulation_run. */
}



