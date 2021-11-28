# MultiThreaded Client and Server

## Overview

MultiThreaded Client and Server is a simulation of the Real world scenario where multiple clients are making requests to a single server.In this program,inorder to give the impression of multiple users from different parts of the world, sending commands to the same server,each client is simulated using a thread which will try to connect with the server independent of each other.The number of client requests throughout the course of the simulation are specified at the start of input.The request is processed at the server side and the corresponding output is sent back to the client.


## Running the Program

* Compile the Server code using `g++ -o server server.cpp -pthread`
* Compile the Client code using `g++ -o client client.cpp -pthread`
* Run the Server using command `./server num_workers` and give an integral input `num_workers`
* Run the Client using command `./client` 
* To stop the Server press Ctrl+C


## Working of Client Program

* Create thread corresponding to each user request that has information of the time when the request is made and command to be sent and make the thread run the function `begin process()` through the below line:
  ```
    pthread_create(&curr_tid, NULL, begin_process, (void *)(thread_input));
  ```

* In the begin process() the client sleeps till the time,request is to be made and then it connects to server using the function `get_socket_fd()` that returns a value which is unique to each of the thread and using function `send_string_on_socket()` the command is sent to the server.

* After processing the command in the server side the string sent by the server is retrieved using the below line of code:
```
    tie(output_msg, num_bytes_read) = read_string_from_socket(socket_fd, buff_sz);
```
   where the output_msg is the message sent by server which will be printed along with the it's original thread_id and the user request number in the input order.

* Once all user threads completed their execution we return from the client program.


## Working of Server Program

* After setting up the server to listen for the client requests using the `bind()` and `listen()` functions,server creates 'n' worker threads where n is given through command line and each worker threads executes a function `worker_job`.
* Now the server is ready to accept requests  from clients with the help of function `accept()` which is a blocking call and the server runs infinitely in a while loop.
* Once a user thread created in the client program makes requests to the server it accepts them and pushes to a Queue and sends the sends signal
 ```
 pthread_cond_signal(&job);
 ```
  to the worker threads.
* The worker thread that recieves the signal first will pop the socket_fd from the Queue and process the request using `handle_connection()` function.
* Once it finishes processing again it will check for the presence of client request in Queue and go to conditional wait using below line of code again until a signal of client request arrival is sent.
```
 pthread_cond_wait(&job, &que_pop_mutex);
``` 
* As the Queue pushing and poping is non-synchronous with many threads popping out and main function pushing it mutex_locks are maintained to avoid this problem.
* A mutex named *que_push_mutex* is maintained to ensure synchronized push of elements to the queue and access them as well and another mutex named *que_pop_mutex* is added so that no two threads can't pop out an element from the Queue simulatenously.
* In the `handle connection()` only a single request per user is handled where the command sent through the string from client is tokenized with `tokenise()` function and each operation is one by one checked starting with insert then delete,update,concat and followed by fetch and for each of the operation along with the values provided appropriate message after processing the queries.
* In order to synchronise the operations performed by each thread on the dictionary,a mutex named *dict_mutex* is used which locks the values of vectors while reading or modifying and unlocks after completion.
