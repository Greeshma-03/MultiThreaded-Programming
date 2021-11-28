# The Clasico Experience


## Overview

The Clasico Experience is a simulation of a Real World football match at a stadium.The two teams involved for any match are home team and away team.This program concentrates on the simulation of people coming to the stadium to watch the match, buy tickets to a particular zone, watch the match and then exit.


## Running the Stadium

* Compile the code using command `make`
* Run the program with command `./run`
* Simulation **ends** automatically after all audience left the stadium.


## Working of the Stadium

The main entities in the simulation are:
  - Spectator coming to the stadium
  - Team Goal 

* Input about the number of friends groups and capacities of each zone present in the stadium and the maximum spectating time of any person is provided.
* Information of spectators belonging to each group is given including the team he/she supports followed by the arrival time patience time and then the number of goals they get enraged and each spectator thread is creating whose struct variable stores this informstion.
* The probability of each goal along with the time elapsed and corresponding team is provided and this information is stored in the goal thread.


### Spectator Simulation

* Depending on the team spectator supports, he will be assigned either a home_spectator thread or neutral_spectator or away_spectator and the difference between these three threads is waiting for a seat in different zones.A Home spectator thread waits for a seat in Home_zone and Neutral_zone whereas a Neutral spectator waits for a seat in any of three zones and Away spectator waits for a seat in only away zone.

* For getting a seat by the spectator in any of the eligible zones,the threads corresponding to their zones are created and each zone waits for a seat in that zone. So a Home spectator's thread creates two threads home_zone thread and neutral_zone thread and they will search for seats in their respective zones.

* Each wait_zone is implemented using sem_timedwait() where each thread waits for a ticket only till the patience specific to each person.If there is a ticket available in some zone before the completion of patience time then a global variable *flags[ID]* is changed from 0 to a value represented by that zone.1 corresponds to Home zone and 2 corresponds to Neutral zone and 3 corresponds to Away zone.A mutex is used to synchronize the change of flag by threads corresponding to each person.Suppose if the flag value is not zero then it shouldn't be changed as already a ticket has been discovered by someother thread so we simply sem_post() the seat.

* If no ticket is found by any of the threads for a person before his patience level then the person leaves the stadium and waits for their group of friends.Till then the spectator thread waits for the conditional_signal of either patience time over or got a ticket in any of the eligible zone.

* If a person gets a seat then he will watch match till the spectating time doesn't exceed given limit which is mainatined by array *time_up[]* using a thread that sleeps and changes this flag after sleep and sends a conditional signal.Mean while the spectator threads goes to a conditional wait.Also whenever a goal is scored then each spectator threads recieves a signal except neutral threads and they check if the spectator is enraged or not and accrodingly spectator leaves the match or go to conditional wait again.

* If a person gets a seat and waits at the gate then the seat in corresponding zone is released using `sem_post()`.Whenever each person waits at the gate he will check if he is the last person of the group and accordingly the group either waits for more persons to come or they leave for dinner.


### Goal Simulation

* Each goal thread sleeps till the mentioned time elapse and the probability for a team scoring and missing is decided randomly using `rand()` function and a signal is sent to each of the spectator thread if he is enraged for that goal and after printing the corresponding statement the thread leaves.


### Assumptions

* The input given will be such that all goal scoring chances will be created before the last person exits the simulation.
* The maximum number of people that can come to stadium is 1024.
* The limit on the friends groups in the stadium is 256.
* The name of each spectator doesn't exceed 16 characters.