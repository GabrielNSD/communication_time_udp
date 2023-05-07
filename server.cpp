#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_SIZE 1024

int main()
{
    int server_sockfd, client_sockfd;
    unsigned int server_len;
    socklen_t client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    unsigned short port = 9734;

    if ((server_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("erro na abertura do socket");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    server_len = sizeof(server_address);

    if (bind(server_sockfd, (struct sockaddr *)&server_address, server_len) < 0)
    {
        printf("erro no bind");
        exit(1);
    }

    while (1)
    {
        char buffer[MAX_SIZE];

        printf("Servidor esperando...\n");

        client_len = sizeof(client_address);

        ssize_t receiveResult = recvfrom(server_sockfd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_len);
        if (receiveResult < 0)
        {
            printf("erro no recvfrom");
            exit(1);
        }
        buffer[receiveResult] = '\0';

        ssize_t sendResult = sendto(server_sockfd, &buffer, receiveResult, 0, (struct sockaddr *)&client_address, client_len);
        if (sendResult < 0)
        {
            printf("erro no sendto");
            exit(1);
        }
    }
}