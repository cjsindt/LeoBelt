// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <librealsense2/rs.hpp>
#include <iostream>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    rs2::video_frame *other_frame;
    const char *hello = "Hello from server";
    int count = 0;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
            sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    while(true){
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        read( new_socket , other_frame, 1024);
        std::cout << (*other_frame).get_data() << std::endl;
        //stbi_write_png("socket.png", other_frame->get_width(), other_frame->get_height(),
                //other_frame->get_bytes_per_pixel(), other_frame->get_data(), other_frame->get_stride_in_bytes());
        printf("\n%d\n", count);
        count++;
    }
    return 0;
}
