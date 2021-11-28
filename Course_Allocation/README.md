# Course Allocation Portal

## Overview

Course Allocation Portal is a simulation of Real World application where a student can take trial classes of a course and can withdraw and opt for a different course if he/she does not like the course. Different labs in the college have been asked to provide students who can act as course TA mentors temporarily and take the trial tutorials.


## Running the Portal

* Compile the code using command `make`
* Run the program with command `./run`
* Simulation **ends** automatically after all students exited the simulation.


## Working of the Portal

The main entities in the simulation are:
  - Courses offered in the college
  - Various labs in the college
  - Student mentors for mentoring courses
  - Students participating in the course registration


* The courses present for registration are given in the first few lines of the input and the course threads are created to simulate each of the course.

* The information of students participating in the course simulation including the callibre and the preference course IDs and time he/she filled their details is provided and each corresponding student thread is executed.

* The labs that provide student mentors are given with their names,capacity and max TA-ship of each TA and the information these labs are stored in a struct variable *TA_labs[]* and other variables are updated which are used in the lab simulation later on.

* After all the input is read each thread executes it's own function independently as described below and the simulation ends once all the students exited from the simulation.


### Course and Lab Simulation

* Each course will be selecting a TA from the shortlisted labs for the respective courses using threads which execute a function `course()` that runs till none of the labs is available for allocating a TA to that course.

* The course thread repeatedly loops through each lab and checks for the availability and presence of a TA in the lab using function `Get_availabe_TA()` which returns non-negative index if there is a available TA to be allocated for that course and returns -1 if there is some TA in that particular lab but he is busy in taking tutorial for other course and returns -2 in the case where there are no student mentors available for TA ship and the lab is considered as absent.

* If there is a TA present to take a tutorial in the course then the status of that TA will be changed to -1 as he is busy with the tutorial and he allocates randomly w seats which is synchronized by putting a mutex *course_seat_mutex[ID]* and after the completion of tutorial,again the alloted seats for that course is set to 0 protected by mutex lock and then TA is made available in the lab.

* While looping through each lab for corresponding course if the lab is absent then we skip for checking the availability of each TA else we loop through each TA and if none of them is present i.e completed their TA-ship then we mark the lab as absent and the lab simulation is stopped.

* While reading and modifying the absence of a lab from the array *absent_labs[]* it is synchronized using the mutex *lababsent_mutex* which is common mutex for any lab.

* If all labs are absent corresponding to a course i.e no more TA's are present for furthur TA-ship then the variable `non_existence` used for each course thread becomes equal to the number of labs allocated and then we exit the course simulation i.e the course is withdrawn from the allocation portal.


### Student Simulation

* Each student is simulated using a student thread and the threads sleep till the time the student filled the course preferences and we loop through the 3 preferences of the students.For each of the preference he waits for a tutorial slot in the course that is allocated by a TA.

* It is implemented using busy waiting where each student checks for a allocated seat and also if the course is withdrawn or not from the portal which is checked using varibales *course_interest[]* and *alloted[]* and they are protected by the mutexs for synchronization of student threads accessing and updating the variables simultaneously.

* If the student gets a seat in this prefered course he listens to the tutorial and deciding if he could finalise the course depending on the random probabilitiy and the probability associated with his callibre and course interest.If he finalises the course then he is exited from the simulation else he will go to next preference course and if this is the last prefered course then he exits the simulation.

* If the course is withdrawn then student moves the next prefernce or exits the simulation if that was the last prefered course.


### Assumptions

* The number of courses and labs present in the college doesn't exceed the limit of 64.
* The number of TAs present in each lab are atmost 32.
* The maximum number of characters present in a course name is 32.