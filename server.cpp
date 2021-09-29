#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include "rs232.h"
#include <librealsense2/rs.hpp>

#define PORT     8080
#define IP "192.168.137.210"

// Driver code
int main() {
    int sockfd;
    int count = 0;
    rs2::video_frame * buffer;
    const char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(true){
        int len, n;

        len = sizeof(cliaddr);  //len is value/resuslt
        printf("Waiting...\n");
        recvfrom(sockfd, buffer, 921600,
                 MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                         reinterpret_cast<socklen_t *>(&len));
        printf("Client : \n");
        sendto(sockfd, (const char *)hello, strlen(hello),
               MSG_DONTWAIT, (const struct sockaddr *) &cliaddr,
                       len);
        printf("Hello message sent.\n");
        std::cout << count++ << std::endl;
    }
    return 0;
}