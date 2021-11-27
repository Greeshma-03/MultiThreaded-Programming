#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <semaphore.h>

/////////////////////////////
#include <iostream>
#include <queue>
#include <assert.h>
#include <tuple>
using namespace std;
/////////////////////////////

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

typedef long long LL;

#define pb push_back
#define debug(x) cout << #x << " : " << x << endl
#define part cout << "-----------------------------------" << endl;

///////////////////////////////
#define MAX_CLIENTS 4
#define PORT_ARG 8001

const int initial_msg_len = 256;

////////////////////////////////////

const LL buff_sz = 1048576;

/*********************************************************************************/
pthread_mutex_t que_push_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t que_pop_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t myjob = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dict_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t job = PTHREAD_COND_INITIALIZER;
queue<int> que;                //list of client requests accepted
vector<pair<int, string>> vec; //dictionary stored in server
/*************************************************************************/

///////////////////////////////////////////////////
pair<string, int> read_string_from_socket(const int &fd, int bytes)
{
    std::string output;
    output.resize(bytes);

    int bytes_received = read(fd, &output[0], bytes - 1);
    debug(bytes_received);
    if (bytes_received <= 0)
    {
        cerr << "Failed to read data from socket. \n";
    }

    output[bytes_received] = 0;
    output.resize(bytes_received);
    // debug(output);
    return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s)
{
    // debug(s.length());
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0)
    {
        cerr << "Failed to SEND DATA via socket.\n";
    }

    return bytes_sent;
}

///////////////////////////////

void tokenise(string str, vector<string> &use)
{
    use.resize(0);
    string word = "";
    for (auto x : str)
    {
        if (x == ' ')
        {
            if ((word.find_first_not_of(' ') != std::string::npos) || (word != ""))
                use.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    if (word.find_first_not_of(' ') != std::string::npos)
        use.push_back(word);
}

int get_value(int key)
{
    pthread_mutex_lock(&dict_mutex);
    for (int i = 0; i < vec.size(); i++)
    {
        if (key == vec[i].first)
        {
            pthread_mutex_unlock(&dict_mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&dict_mutex);
    return -1;
}

void handle_connection(int client_socket_fd)
{
    int received_num, sent_num;
    // cout<<"Hi,from: "<<pthread_self()<<endl;
    /* read message from client */
    int ret_val = 1;

    while (true)
    {
        string cmd, msg_to_send_back;
        tie(cmd, received_num) = read_string_from_socket(client_socket_fd, buff_sz);
        ret_val = received_num;
        // debug(ret_val);
        // printf("Read something\n");
        // cout << "Client sent : " << cmd << endl;

        if (ret_val <= 0)
        {
            // perror("Error read()");
            printf("Server could not read msg sent from client\n");
            goto close_client_socket_ceremony;
        }
        vector<string> flags;
        tokenise(cmd, flags);

        cout << "command is: " << cmd << endl;
        cout << "flags: " << endl;
        for (auto it : flags)
            cout << it << endl;

        if (flags[0] == "insert")
        {
            if (flags.size() != 3)
            {
                msg_to_send_back = "Incorrect input!!";
            }
            else
            {
                int key = stoi(flags[1]);
                string val = flags[2];
                if (get_value(key) == -1)
                {
                    pthread_mutex_lock(&dict_mutex);
                    vec.push_back({key, val});
                    pthread_mutex_unlock(&dict_mutex);
                    msg_to_send_back = "Insertion successful";
                }
                else
                {
                    msg_to_send_back = "Key already exists";
                }
            }
        }
        else if (flags[0] == "delete")
        {
            if (flags.size() != 2)
            {
                msg_to_send_back = "Incorrect input!!";
            }
            else
            {
                int key = stoi(flags[1]);
                if (get_value(key) == -1)
                {
                    msg_to_send_back = "No such key exists";
                }
                else
                {
                    int i = get_value(key);
                    pthread_mutex_lock(&dict_mutex);
                    vec.erase(vec.begin() + i);
                    pthread_mutex_unlock(&dict_mutex);

                    msg_to_send_back = "Deletion successful";
                }
            }
        }
        else if (flags[0] == "update")
        {
            if (flags.size() != 3)
            {
                msg_to_send_back = "Incorrect input!!";
            }
            else
            {
                int key = stoi(flags[1]);
                string val = flags[2];

                if (get_value(key) == -1)
                {
                    msg_to_send_back = "Key does not exist";
                }
                else
                {
                    int vals = get_value(key);
                    pthread_mutex_lock(&dict_mutex);
                    vec[vals].second = val;
                    pthread_mutex_unlock(&dict_mutex);

                    msg_to_send_back = val;
                }
            }
        }
        else if (flags[0] == "concat")
        {
            if (flags.size() != 3)
            {
                msg_to_send_back = "Incorrect input!!";
            }
            else
            {
                int key1 = stoi(flags[1]);
                int key2 = stoi(flags[2]);
                int i1 = get_value(key1);
                int i2 = get_value(key2);

                if (i1 == -1 || i2 == -1)
                {
                    msg_to_send_back = "Concat failed as at least one of the keys does not exist";
                }
                else
                {
                    pthread_mutex_lock(&dict_mutex);
                    string val1 = vec[i1].second;
                    string val2 = vec[i2].second;
                    vec[i1].second = val1 + val2;
                    vec[i2].second = val2 + val1;
                    pthread_mutex_unlock(&dict_mutex);
                    msg_to_send_back = val2 + val1;
                }
            }
        }
        else if (flags[0] == "fetch")
        {
            if (flags.size() != 2)
            {
                msg_to_send_back = "Incorrect input!!";
            }
            else
            {
                int key = stoi(flags[1]);
                int i = get_value(key);
                pthread_mutex_lock(&dict_mutex);
                msg_to_send_back = vec[i].second;
                pthread_mutex_unlock(&dict_mutex);
            }
        }
        else
        {
            msg_to_send_back = "Incorrect";
        }
        // if (cmd == "exit")
        // {
        //     cout << "Exit pressed by client" << endl;
        //     goto close_client_socket_ceremony;
        // }
        // string msg_to_send_back = "Ack: " + cmd;

        ////////////////////////////////////////
        // "If the server write a message on the socket and then close it before the client's read. Will the client be able to read the message?"
        // Yes. The client will get the data that was sent before the FIN packet that closes the socket.

        int sent_to_client = send_string_on_socket(client_socket_fd, msg_to_send_back);
        // debug(sent_to_client);
        if (sent_to_client == -1)
        {
            perror("Error while writing to client. Seems socket has been closed");
            goto close_client_socket_ceremony;
        }
        break;
    }
close_client_socket_ceremony:
    close(client_socket_fd);
    printf(BRED "Disconnected from client" ANSI_RESET "\n");
    // return NULL;
}

void *worker_job(void *arg)
{
    pthread_mutex_lock(&que_pop_mutex);
    while (1)
    {
        //each thread just lock the que_mutex
        int client_socket_fd = -1;
        //get the available job from que of jobs
        if (!que.empty())
        {
            pthread_mutex_lock(&que_push_mutex);
            client_socket_fd = que.front();
            que.pop();
            pthread_mutex_unlock(&que_push_mutex);

            pthread_mutex_unlock(&que_pop_mutex);
            handle_connection(client_socket_fd);
        }
        else
        {
            pthread_cond_wait(&job, &que_pop_mutex);
        }
        //unlock the que_mutex
        //if there is a job execute handle_connection
        //else conditional wait
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int wel_socket_fd /*listens to the requests using this socket*/, client_socket_fd /*Communicates to the client using this scoket*/, port_number /*port number used by server to listen to incoming requests*/;
    socklen_t clilen;

    struct sockaddr_in serv_addr_obj, client_addr_obj;
    /////////////////////////////////////////////////////////////////////////
    /* create socket */
    /*
    The server program must have a special door—more precisely,
    a special socket—that welcomes some initial contact 
    from a client process running on an arbitrary host
    */
    //get welcoming socket
    //get ip,port
    /////////////////////////
    wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0); //intialsing a socket struct
    if (wel_socket_fd < 0)
    {
        perror("ERROR creating welcoming socket");
        exit(-1);
    }

    //////////////////////////////////////////////////////////////////////
    /* IP address can be anything (INADDR_ANY) */
    bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj)); //to specify listening addresses,ports a struct is used and intilaise to 0
    port_number = PORT_ARG;                               //port used to listen to incoming requests
    serv_addr_obj.sin_family = AF_INET;                   //AF_INET specifies whether u need ipv4 or ipv6 while communicating
    // On the server side I understand that INADDR_ANY will bind the port to all available interfaces,
    serv_addr_obj.sin_addr.s_addr = INADDR_ANY;  //mention a specific address you wanted to listen or listen to any addresses present over netwroking interface for ur local machine
    serv_addr_obj.sin_port = htons(port_number); //process specifies port(big-endian and small-endian conversion)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* bind socket to this port number on this machine */
    /*When a socket is created with socket(2), it exists in a name space
       (address family) but has no address assigned to it.  bind() assigns
       the address specified by addr to the socket referred to by the file
       descriptor wel_sock_fd.  addrlen specifies the size, in bytes, of the
       address structure pointed to by addr.  */

    //CHECK WHY THE CASTING IS REQUIRED
    //bind the socket with all initilisations so far we created
    if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj, sizeof(serv_addr_obj)) < 0)
    {
        perror("Error on bind on welcome socket: ");
        exit(-1);
    }
    //////////////////////////////////////////////////////////////////////////////////////

    /* listen for incoming connection requests */

    listen(wel_socket_fd, MAX_CLIENTS); //MAX-CLIENTS you can keep them in que and 1000's of clients came just reject lol
    cout << "Server has started listening on the LISTEN PORT" << endl;
    clilen = sizeof(client_addr_obj);

    int num = atoi(argv[1]);
    pthread_t user_threads[num]; //'num' number of user threads
    for (int i = 0; i < num; i++)
    {
        pthread_t curr_tid;
        pthread_create(&curr_tid, NULL, worker_job, (void *)(long long int)i);
    }

    while (1)
    {
        /* accept a new request, create a client_socket_fd */
        /*
        During the three-way handshake, the client process knocks on the welcoming door
of the server process. When the server “hears” the knocking, it creates a new door—
more precisely, a new socket that is dedicated to that particular client. 
        */
        //**********accept is a blocking call***********
        printf("Waiting for a new client to request for a connection\n");
        client_socket_fd = accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
        if (client_socket_fd < 0)
        {
            perror("ERROR while accept() system call occurred in SERVER");
            //destroy everything before exit
            exit(-1);
        }

        //we need to send signal to worker threads that some job has come
        //no worker thread can pop some job in the mean while time

        pthread_mutex_lock(&que_push_mutex);
        que.push(client_socket_fd);
        pthread_mutex_unlock(&que_push_mutex);

        pthread_cond_signal(&job); //sending signal that job arrived

        printf(BGRN "New client connected from port number %d and IP %s \n" ANSI_RESET, ntohs(client_addr_obj.sin_port), inet_ntoa(client_addr_obj.sin_addr));
    }

    close(wel_socket_fd);
    return 0;
}
