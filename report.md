# The Clasico Experiment

## Runnning Program

* To compile code use command: `gcc -g -pthread q2.c`
* To run the code use command: `./a.out`


### Introduction

* A football match is taking place at the Gachibowli Stadium. There are 2 teams involved: FC Messilona (Home Team) and Benzdrid CF (Away Team). People from all over the city are coming to the stadium to watch the match. You have to create a simulation where people enter the stadium, buy tickets to a particular zone (stand), watch the match and then exit.

* The main entities involved are person who came to stadium to watch the match and the goal scoring chance after the mentioned time.Both these entities are simulated using the threads and logic behind the implementation is discuused below.


### Person simulation

* For each person a thread is created that executes a function `allocates seats` which allocates seats randomly depending on the zone a person is eligible.

* To check the availability of seats in each of the eligible zones the threads creates other threads which will search for a position in each of the eligible zone and waits till the join of the threads.

* If a person can get ticket in more than 1 zone then the zone which is available first is taken by the person and the otehr is seat is released which is done using `sem_timedwait()` that waits only till the patiemce time of the person and the time of capture of seat is stored to compare for the seat allocation.

* If the person didn't get any seat then he/she will wait for their friends group at the exit gate else the person watches the match till either he gets enraged or the maximum spectating time is reached.

* In order to check if time for spectating is left or not for a person one more thread is create that sleeps for spectating time and signals that the time is over and the person leaves accordingly else if due to scoring a goal a person is enraged then also he may leave so a common signal for goal scoring and as well as completion of spectating time is maintained.

* The person waits at the exit gate till all members of the group has reached at the exit gate and then the entire group of friends will go to dinner.

* Once all the persons leave the exit gate then the simulation will ge exited.


### Goal simulation

* The goal thread sleeps till the time given through input and then it decides whether a team scores a goal or not randomly using the chance given in the input and generating value between 0 and 1 and checking accordingly.

* If a goal is scored then the signal is sent to each of the person to know if he/she is enraged or not.The thread checks and if he/she is enraged then it leaves the stadium and waits at the gate for their friends group.


### Assumptions Made

* The maximum number of people entering the stadium shouldn't exceed 1000.
* The maximum number of groups entering the stadium shouldn't exceed 1000.
* A person's name entering the stadium has maximum number of characters as 20.
