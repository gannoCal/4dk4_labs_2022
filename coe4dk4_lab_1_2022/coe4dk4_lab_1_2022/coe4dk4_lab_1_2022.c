
/*
 *
 * Simulation of Single Server Queueing System
 * 
 * Copyright (C) 2014 Terence D. Todd Hamilton, Ontario, CANADA,
 * todd@mcmaster.ca
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/*******************************************************************************/

#include <stdio.h>
#include "simlib.h"
//#include <matplotlibcpp.h>

/*******************************************************************************/

/*
 * Simulation Parameters
 */

#define RANDOM_SEED 12345611
#define NUMBER_TO_SERVE 1e6

#define SERVICE_TIME 10
#define ARRIVAL_RATE 0.1

#define BLIP_RATE 10000

#define COUNT 80
#define START 0

#define MAX_QUEUE_SIZE 1000

/*******************************************************************************/

/*
 * main() uses various simulation parameters and creates a clock variable to
 * simulate real time. A loop repeatedly determines if the next event to occur
 * is a customer arrival or customer departure. In either case the state of the
 * system is updated and statistics are collected before the next
 * iteration. When it finally reaches NUMBER_TO_SERVE customers, the program
 * outputs some statistics such as mean delay.
 */

int main()
{
    char* path = "C:\\\"Program Files\"\\gnuplot\\bin\\gnuplot";
    
    int num_to_count[COUNT];
    int seeds[COUNT];
    //int max_queue[COUNT] = {10, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300 };
    int max_queue[COUNT];
    int total_arrived_arr[COUNT];
    double total_rejected_probability_arr[COUNT];
    //double arrival_rates[COUNT] = { 0.005,  0.055, 0.105, 0.155, 0.205, 0.255, 0.305, 0.355, 0.405, 0.455, 0.505, 0.555, 0.605, 0.655, };
    //double arrival_rates[COUNT] = { 1.005, 1.01, 1.015, 1.02 };
    double arrival_rates[COUNT];
    double mean_delays[COUNT];
    double mean_in_system[COUNT];
    for (int i = 0; i < COUNT; i++) {
        //int upper = (1.0/SERVICE_TIME) * 10000; // * 10e-5
        //int lower = 0.01 * 10000;
        //int num = (rand() % (upper - lower + 1)) + lower;
        //arrival_rates[i] = ((i + 0.5)/1.4) * (SERVICE_TIME / (COUNT * COUNT)) + 0.2;
        //arrival_rates[i] = i * (0.033333 / 10.0) + (0.033333 / 20.0);
        //arrival_rates[i] = 0.1 + 0.1 + 0.025;
        if (i == 0) arrival_rates[i] = 0.01;
        else arrival_rates[i] = arrival_rates[i-1] + 0.00225;

        //if (i == 0) max_queue[i] = 10;
        //else max_queue[i] = max_queue[i-1]*2;
        max_queue[i] = -1;

        if (i == 0) seeds[i] = 400137271;
        else seeds[i] = (rand() % (4000000000 - 1 + 1)) + 1;
        
        //printf("upper = %d\n", upper);
        //printf("lower = %d\n", lower);
        //printf("num = %d\n", num);
        num_to_count[i] = NUMBER_TO_SERVE;
        //if (i == 0) num_to_count[i] = 10e3;
        //else num_to_count[i] = num_to_count[i - 1] + 10e3;

        //if (i == 0) arrival_rates[i] = 0.1 + 0.1 + 0.025;
        //else arrival_rates[i] = arrival_rates[i - 1] + 0.01;

        printf("arrival_rates = %f\n", arrival_rates[i]);
    }
    //exit(1);
    /* Set the seed of the random number generator. */
    

    double clock = 0; /* Clock keeps track of simulation time. */

    /* System state variables. */
    int number_in_system = 0;
    double next_arrival_time = 0;
    double next_departure_time = 0;

    /* Data collection variables. */
    long int total_served = 0;
    long int total_arrived = 0;
    long int total_rejected = 0;

    double total_busy_time = 0;
    double integral_of_n = 0;
    double last_event_time = 0;
    int seed_sim = 0;
    int max_sim = 0;
    for (seed_sim = START; seed_sim < COUNT; seed_sim++) {
    //for (max_sim = START; max_sim < COUNT; max_sim++) {
        for (int sim = START; sim < COUNT; sim++) {

            clock = 0; /* Clock keeps track of simulation time. */

            /* System state variables. */
            number_in_system = 0;
            next_arrival_time = 0;
            next_departure_time = 0;

            /* Data collection variables. */
            total_served = 0;
            total_arrived = 0;
            total_rejected = 0;

            total_busy_time = 0;
            integral_of_n = 0;
            last_event_time = 0;

            double arrival_rate = arrival_rates[sim];

            random_generator_initialize(seeds[seed_sim]);



            /* Process customers until we are finished. */
            while (total_served < num_to_count[sim]) {

                /* Test if the next event is a customer arrival or departure. */
                if (number_in_system == 0 || next_arrival_time < next_departure_time) {

                    /*
                     * A new arrival is occurring.
                     */

                    clock = next_arrival_time;
                    //next_arrival_time = clock + exponential_generator((double)1 / arrival_rate);
                    next_arrival_time = clock + ((double)1 / arrival_rate);

                    /* Update our statistics. */
                    integral_of_n += number_in_system * (clock - last_event_time);
                    last_event_time = clock;

                    if (number_in_system < max_queue[max_sim] || max_queue[max_sim] == -1) number_in_system++;
                    else total_rejected++;
                    total_arrived++;

                    /* If this customer has arrived to an empty system, start its
                   service right away. */
                    //if (number_in_system == 1) next_departure_time = clock + exponential_generator((double)SERVICE_TIME);
                    if (number_in_system == 1) next_departure_time = clock + SERVICE_TIME;

                }
                else {

                    /*
                     * A customer departure is occuring.
                     */
                    //double next_service_time = exponential_generator((double)SERVICE_TIME);
                    double next_service_time = SERVICE_TIME;
                    clock = next_departure_time;

                    /* Update our statistics. */
                    integral_of_n += number_in_system * (clock - last_event_time);
                    last_event_time = clock;

                    number_in_system--;
                    total_served++;
                    total_busy_time += next_service_time;

                    /*
                     * If there are other customers waiting, start one in service
                     * right away.
                     */

                    if (number_in_system > 0) next_departure_time = clock + next_service_time;

                    /*
                     * Every so often, print an activity message to show we are active.
                     */

                    if (total_served % BLIP_RATE == 0)
                        printf("RUN: %d : Customers served = %ld (Total arrived = %ld)\r",
                            sim, total_served, total_arrived);
                }

            }

            mean_delays[sim] = integral_of_n / total_served;
            mean_in_system[sim] = integral_of_n / clock;
            total_arrived_arr[sim] = total_arrived;
            total_rejected_probability_arr[sim] = (double)((double)total_rejected)/ ((double)total_arrived);

        }
        FILE* fp;
        fp = fopen("Output.csv", "a");
        for (int i = START; i < COUNT; i++) fprintf(fp, "%f,%f,%d,%d,%d,%f,%f,%d\n", arrival_rates[i], mean_delays[i], seeds[seed_sim], num_to_count[i], total_arrived_arr[i], mean_in_system[i], total_rejected_probability_arr[i], max_queue[max_sim]);
        fflush(fp);
    }
  /* Output final results. */
  printf("\nUtilization = %f\n", total_busy_time/clock);
  printf("Fraction served = %f\n", (double) total_served/total_arrived);
  printf("Mean number in system = %f\n", integral_of_n/clock);
  printf("integral_of_n = %f\n", integral_of_n);
  printf("Mean delay = %f\n", integral_of_n/total_served);
  printf("Arrival Rate = %f\n", arrival_rates[COUNT-1]);

  //double x[COUNT];
  //double y[COUNT];
  double x[COUNT];
  double y[COUNT];
  *x = *arrival_rates;
  *y = *mean_delays;
  //FILE* gnuplot = _popen(path, "w");
  ////printf(gnuplot, "set format y \"%s\"; set ytics(500, 1000)\n");
  ////printf(gnuplot, "show ytics\n");
  //fprintf(gnuplot, "plot '-' using 1:2:(sprintf(\"(%%d, %%d)\", $1, $2)) with labels notitle, using ($1/1000000.):2 with linespoints ls 1 linetype 1 linewidth 2 pointtype 7 pointsize 1.5 \n");
  //for (int i = START; i < COUNT; i++) {
  //    printf("%g %g\n", x[i], y[i]);
  //    fprintf(gnuplot, "%f %g\n", x[i], y[i]);
  //}
  //fprintf(gnuplot, "e\n");
  //fflush(gnuplot);
  


  /* Halt the program before exiting. */
  printf("Hit Enter to finish ... \n");
  getchar(); 


  return 0;

}






