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

/////////////////////////////
#include <pthread.h>
#include <thread>
#include <iostream>
#include <semaphore.h>
#include <assert.h>
#include <queue>
#include <vector>
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
const LL MOD = 1000000007;
#define part cout << "-----------------------------------" << endl;
#define pb push_back
#define debug(x) cout << #x << " : " << x << endl
#define MAX 1024

typedef struct thread_details
{
    int ID;
    int time;
    string str;
} td;

///////////////////////////////
#define SERVER_PORT 8001
////////////////////////////////////

const LL buff_sz = 1048576;
///////////////////////////////////////////////////
pair<string, int> read_string_from_socket(int fd, int bytes)
{
    std::string output;
    output.resize(bytes);

    int bytes_received = read(fd, &output[0], bytes - 1);
    // debug(bytes_received);
    if (bytes_received <= 0)
    {
        cerr << "Failed to read data from socket. Seems server has closed socket\n";
        // return "
        exit(-1);
    }

    // debug(output);
    output[bytes_received] = 0;
    output.resize(bytes_received);

    return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s)
{
    // cout << "We are sending " << s << endl;
    int bytes_sent = write(fd, s.c_str(), s.length());
    // debug(bytes_sent);
    // debug(s);
    if (bytes_sent < 0)
    {
        cerr << "Failed to SEND DATA on socket.\n";
        // return "
        exit(-1);
    }

    return bytes_sent;
}

int get_socket_fd(struct sockaddr_in *ptr)
{
    struct sockaddr_in server_obj = *ptr;

    // socket() creates an endpoint for communication and returns a file
    //        descriptor that refers to that endpoint.  The file descriptor
    //        returned by a successful call will be the lowest-numbered file
    //        descriptor not currently open for the process.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Error in socket creation for CLIENT");
        exit(-1);
    }
    /////////////////////////////////////////////////////////////////////////////////////
    int port_num = SERVER_PORT;

    memset(&server_obj, 0, sizeof(server_obj)); // Zero out structure
    server_obj.sin_family = AF_INET;
    server_obj.sin_port = htons(port_num); //convert to big-endian order

    // Converts an IP address in numbers-and-dots notation into either a
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    //https://stackoverflow.com/a/20778887/6427607

    /////////////////////////////////////////////////////////////////////////////////////////
    /* connect to server */

    if (connect(socket_fd, (struct sockaddr *)&server_obj, sizeof(server_obj)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(-1);
    }

    //part;
    // printf(BGRN "Connected to server\n" ANSI_RESET);
    // part;
    return socket_fd;
}
////////////////////////////////////////////////////////

void *begin_process(void *inp)
{
    int ID=((struct thread_details *)inp)->ID;
    int time = ((struct thread_details *)inp)->time;
    string to_send = ((struct thread_details *)inp)->str;
    struct sockaddr_in server_obj;

    sleep(time); //wait till this time to connect to the server
    int socket_fd = get_socket_fd(&server_obj);
    // cout << "Connection to server successful" << endl;

    //sending the data to server
    send_string_on_socket(socket_fd, to_send);

    //reading the data from server
    int num_bytes_read;
    string output_msg;
    tie(output_msg, num_bytes_read) = read_string_from_socket(socket_fd, buff_sz);
    std::thread::id Thread_ID = std::this_thread::get_id();

    cout <<ID<<":"<<Thread_ID<<":"<< output_msg << endl;
    return NULL;
}


int main(int argc, char *argv[])
{
    //taking the input from client
    //****************************
    int m;
    cin >> m;
    vector<string>data(m+1);
    for (int i = 0; i <= m; i++)
    {
        getline(cin, data[i]);
    }
    //*****************************
    //*****************************

    pthread_t user_threads[m+1]; //'m' number of user threads
    for (int i = 1; i <= m; i++)
    {
        char use[MAX]="";
        int num=0;
        for (auto &it : data[i])
        {
            if (it != ' ')
            {
                use[num++]=it;
                it=' ';
            }
            else
            {
                break;
            }
        }
        string to_send=data[i];
        pthread_t curr_tid;
        td *thread_input = (td *)(malloc(sizeof(td)));
        thread_input->time = atoi(use);
        thread_input->str=to_send;
        thread_input->ID=i-1;
        pthread_create(&curr_tid, NULL, begin_process, (void *)(thread_input));
        user_threads[i] = curr_tid;
        // cout << i << ": " << data[i] << " ";

    }

    for (int i = 1; i <=m; i++)
    {
        pthread_join(user_threads[i], NULL);
    }
    // cout<<"I am leaving as all jobs done!!"<<endl;
    return 0;
}