#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <fstream>

using std::cout;
using std::endl;

// accepted argumens argv[1] = ip, argv[2] = data_size
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Usage: ./client <ip> <data_size>" << endl;
        return 0;
    }

    int sockfd;
    int len, len_recv;
    struct sockaddr_in address;
    int result;
    int data_size = atoi(argv[2]);
    char data[data_size];

    std::memset(data, 'a', data_size);

    unsigned short port = 9734;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        cout << "socket creation failed..." << endl;
        exit(0);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(argv[1]);
    address.sin_port = htons(port);

    len = sizeof(address);
    len_recv = sizeof(address);

    struct timeval timeout;
    timeout.tv_sec = 3; // Set timeout to 3 seconds
    timeout.tv_usec = 0;

    int count = 0;

    while (count < 30)
    {
        if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        {
            cout << "setsockopt failed" << endl;
            return 1;
        }

        struct timeval time_send, time_receive;

        gettimeofday(&time_send, NULL);
        ssize_t sendResult = sendto(sockfd, data, sizeof(data), 0, (struct sockaddr *)&address, len);
        if (sendResult == -1)
        {
            cout << "sendto failed" << endl;
            close(sockfd);
            return 1;
        }
        ssize_t receiveResult = recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&address, (socklen_t *)&len_recv);
        if (receiveResult == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                cout << "Timeout reached. No data received." << endl;
                exit(1);
            }
            else
            {
                cout << "recvfrom failed" << endl;
            }
        }
        else
        {
            gettimeofday(&time_receive, NULL);
            // cout << "Data received: " << data << endl;
            int microseconds = (time_receive.tv_sec - time_send.tv_sec) * 1000000 + ((int)time_receive.tv_usec - (int)time_send.tv_usec);
            struct timeval tc;
            tc.tv_sec = microseconds / 1000000;
            tc.tv_usec = microseconds % 1000000;
            cout << "Time elapsed: " << tc.tv_sec << "s " << tc.tv_usec << "us" << endl;

            // Save elapsed time to a file
            std::ofstream file("elapsed_time.txt", std::ios::app);
            if (file.is_open())
            {
                // Structure of the file
                // <seconds>; <microseconds>; <ip>; <data_size>;
                file << tc.tv_sec << "; " << tc.tv_usec << ";" << argv[1] << ";" << data_size << ";" << endl;
                file.close();
                cout << "Elapsed time saved to elapsed_time.txt" << endl;
            }
            else
            {
                cout << "Unable to open elapsed_time.txt for writing." << endl;
            }
        }
        count++;
    }

    return 0;
}