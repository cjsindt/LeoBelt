// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp>
#include "common/example.hpp"
#include "third-party/imgui/imgui.h" // <imgui.h>
#include "third-party/imgui/imgui_impl_glfw.h"
#include <SFML/Graphics.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <cmath>
#include <time.h>
#include <iomanip>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "rs232.h"
#include <chrono>

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
  
#define PORT     8080
#define MAXLINE 1024
#define serverIP "192.168.137.210"

#define BUF_SIZE 123
char str_send[2048][BUF_SIZE]; // send data buffer
int cport_nr = 16;

float get_depth_scale(rs2::device dev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
int* printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale);
void silenceAllFeathers();
void testButton();

int main(int argc, char * argv[]) try
{
    int bdrate = 57600;
    
    char mode[] ={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
    unsigned char str_recv[BUF_SIZE]; // recv data buffer
    std::string intensityValue = "";
    // Strings that will be sent over serial to the Master Feather
    // Format is "Intensity-Intensity-Feather#"
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 256; j++){
            if(j % 52 < 26){
                intensityValue += (char)((j%52)+65);
            } else {
                intensityValue += (char)(((j-26)%52)+97);
            }
            if(j < 52){
                intensityValue += '#';
            } else if(j < 104){
                intensityValue += '$';
            } else if(j < 156){
                intensityValue += '%';
            } else if(j < 208){
                intensityValue += '&';
            } else if(j < 256){
                intensityValue += '\'';
            }
            intensityValue += std::to_string(i+1);
            strcpy(str_send[256*i+j], intensityValue.c_str());
            intensityValue = "";
        }
    }
    
    if(RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
        //return(0);
    }
    
    usleep(2000000); // Waits 2s for stable condition

    // Create Window
    sf::RenderWindow app(sf::VideoMode(640, 480), "LEO Belt"); // Simple window handling
    app.setVerticalSyncEnabled(false);
    
    // Create a pipeline to easily configure and start the camera
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, -1, 640, 480, RS2_FORMAT_ANY, 30);
    cfg.enable_stream(RS2_STREAM_COLOR, -1, 640, 480, RS2_FORMAT_RGBA8, 30);
    rs2::pipeline pipe;
    
    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start(cfg);

    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    // The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(align_to);
    
    // Create objects for heatmap
    sf::Vector2u size = app.getSize();
    int width = size.x;
    int height = size.y;
    sf::Uint8* pixels = new sf::Uint8[width * height * 4];
    sf::Texture t1, t7;
    sf::Texture video;
    video.create(640,480);
    t1.loadFromFile("white-square.png", sf::IntRect(0,0,width/3,height/3));
    t7.loadFromFile("white-square.png", sf::IntRect(0,0,width/2,height/3));
    
    sf::Sprite s1(t1), s2(t1), s3(t1), s4(t1), s5(t1), s6(t1), s7(t7), s8(t7);

    int getFrame = 0;
    int* c;
    /*
    // Client-side socket
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    rs2::video_frame *frame;
    int *sockTest;
    int sockint = 5;
    sockTest = &sockint;
    int len;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(serverIP);*/
    
    // Rapid buzz to indicate device is on and all feathers are functioning
    for (int i=1; i<=8; i++) {
        RS232_cputs(cport_nr, str_send[256*i-1]);
        usleep(500000);
        RS232_cputs(cport_nr,str_send[256*(i-1)]);
        usleep(200000);
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    while (app.isOpen()) // Application still alive?
    {        
        sf::Event e;
        while (app.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                app.close();
            }
        }
        
        rs2::frameset frameset;
        try {
            // Using the align object, we block the application until a frameset is available
            frameset = pipe.wait_for_frames();
        }
        catch (const std::exception& e) {
            std::cout << e.what();
        }

        if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = pipe.get_active_profile();
            align_to = find_stream_to_align(profile.get_streams());
            align = rs2::align(align_to);
            depth_scale = get_depth_scale(profile.get_device());
        }
    
        // Trying to get both other and aligned depth frames
        rs2::video_frame other_frame = frameset.first(align_to);
        rs2::depth_frame aligned_depth_frame = frameset.get_depth_frame();
    
        //If one of them is unavailable, continue iteration
        if (!aligned_depth_frame || !other_frame)
        {
            continue;
        }
        
      
        // Obtaining pixel depth and heat map specifications
        c = printPixelDepth(other_frame, aligned_depth_frame, depth_scale);

        pixels = const_cast<sf::Uint8*>(reinterpret_cast<const sf::Uint8*>(other_frame.get_data()));
        // Set colors for heat map quadrants
        s1.setColor(sf::Color(*(c+0), 0, *(c+8), 120));
        s2.setColor(sf::Color(*(c+1), 0, *(c+9), 120));
        s3.setColor(sf::Color(*(c+2), 0, *(c+10), 120));
        s4.setColor(sf::Color(*(c+3), 0, *(c+11), 120));
        s5.setColor(sf::Color(*(c+4), 0, *(c+12), 120));
        s6.setColor(sf::Color(*(c+5), 0, *(c+13), 120));
        s7.setColor(sf::Color(*(c+6), 0, *(c+14), 120));
        s8.setColor(sf::Color(*(c+7), 0, *(c+15), 120));
        
        // Creating the heat map
        s1.setPosition(0,0);
        s2.setPosition(width/3,0);
        s3.setPosition(2*width/3,0);
        s4.setPosition(0,height/3);
        s5.setPosition(width/3,height/3);
        s6.setPosition(2*width/3,height/3);
        s7.setPosition(0,2*height/3);
        s8.setPosition(width/2,2*height/3);

        video.update(pixels);
        sf::Sprite image(video);
        
        app.clear();
            app.draw(image);
            app.draw(s1);
            app.draw(s2);
            app.draw(s3);
            app.draw(s4);
            app.draw(s5);
            app.draw(s6);
            app.draw(s7);
            app.draw(s8);
        app.display();
        
        /*// Send info over socket
        frame = &other_frame;
        sendto(sock , frame , (*frame).get_data() , MSG_DONTWAIT, (const struct sockaddr *) &serv_addr, sizeof(serv_addr) );
        printf("Frame sent\n");
        recvfrom(sock, (char *) buffer, 1024, MSG_DONTWAIT, (struct sockaddr *) &serv_addr, reinterpret_cast<socklen_t*>(&len));
        std::cout << "Server: " << buffer << std::endl;
        std::cout << getFrame << std::endl;*/
        
        getFrame++;
    }
    usleep(1000000);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop-start);
    std::cout << duration.count() << std::endl;
    std::cout << getFrame << std::endl;
    silenceAllFeathers();
    return EXIT_SUCCESS;
}

catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception & e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}

void silenceAllFeathers(){
    RS232_cputs(cport_nr, str_send[0]);
    RS232_cputs(cport_nr, str_send[256]);
    RS232_cputs(cport_nr, str_send[2*256]);
    RS232_cputs(cport_nr, str_send[3*256]);
    RS232_cputs(cport_nr, str_send[4*256]);
    RS232_cputs(cport_nr, str_send[5*256]);
    RS232_cputs(cport_nr, str_send[6*256]);
    RS232_cputs(cport_nr, str_send[7*256]);
}

int* printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale) {

    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    float closest[8] = {10,10,10,10,10,10,10,10};
    static int color[16] = {};
    
    int x_left = 96.97;
    int x_right = 135.76;
    int y_offset = 78.37;

#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];
            
            auto offset = depth_pixel_index*other_bpp;
            
            if ((x>x_left) && (y>y_offset) && (x<(width-x_left-x_right)/3+x_left) && (y<(height-2*y_offset)/3+y_offset)) { // Top-left quadrant -- Feather 1
                if (pixels_distance < closest[0] && pixels_distance > .001){
                    closest[0] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/3+x_left) && (y>y_offset) && (x<2*(width-x_left-x_right)/3+x_left) && (y<(height-2*y_offset)/3+y_offset)) { // Top-middle quadrant -- Feather 2
                if (pixels_distance < closest[1] && pixels_distance > .001){
                    closest[1] = pixels_distance;
                }
            } else if ((x>2*(width-x_left-x_right)/3+x_left) && (y>y_offset) && (x<width-x_right) && (y<(height-2*y_offset)/3+y_offset)) { // Top-right quadrant -- Feather 3
                if (pixels_distance < closest[2] && pixels_distance > .001){
                    closest[2] = pixels_distance;
                }
            } else if ((x>x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<(width-x_left-x_right)/3+x_left) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-left quadrant -- Feather 4
                if (pixels_distance < closest[3] && pixels_distance > .001){
                    closest[3] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/3+x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<2*(width-x_left-x_right)/3+x_left) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-middle quadrant -- Feather 5
                if (pixels_distance < closest[4] && pixels_distance > .001){
                    closest[4] = pixels_distance;
                }
            } else if ((x>2*(width-x_left-x_right)/3+x_left) && (y>(height-2*y_offset)/3+y_offset) && (x<width-x_right) && (y<2*(height-2*y_offset)/3+y_offset)) { // Middle-right quadrant -- Feather 6
                if (pixels_distance < closest[5] && pixels_distance > .001){
                    closest[5] = pixels_distance;
                }
            } else if ((x>x_left) && (y>2*(height-2*y_offset)/3+y_offset) && (x<(width-x_left-x_right)/2+x_left) && (y<height-y_offset)) { // Bottom-left quadrant -- Feather 7 
                if (pixels_distance < closest[6] && pixels_distance > .001){
                    closest[6] = pixels_distance;
                }
            } else if ((x>(width-x_left-x_right)/2+x_left) && (y>2*(height-2*y_offset)/3+y_offset) && (x<width-x_right) && (y<height-y_offset)) { // Bottom-right quadrant -- Feather 8 
                if (pixels_distance < closest[7] && pixels_distance > .001){
                    closest[7] = pixels_distance;
                }
            }
        }
    }
    
    // converting physical distance to 0-255 scale
    for(int i=0; i<=7; i++) {
        //std::cout << "feather: " << i+1 << " distance: " << closest[i];
        closest[i] = 255 + ((0-255.0)*(closest[i]-.001)/(2.0-.001));
        closest[i] = trunc(closest[i]);
        //std::cout << " scaled: " << closest[i] << std::endl;
    }
    
    // Obtaining (R)G(B) color values for each quadrant in heat map
    for(int i=0; i<=7; i++) {
        if(closest[i]<=255 && closest[i]>=0) {
            color[i] = closest[i];
            color[i+8] = 255 - closest[i];
        }
        else if (closest[i]==-1020) {
            color[i] = 255;
            color[i+8] = 0;
            closest[i] = 255;
        }
        // Anything greater than 2m away will remain blue on map
        else {
            color[i] = 0;
            color[i+8] = 255;
        }
        //std::cout << "red: " << color[i] << " blue: " << color[i+8] << std::endl;
    }
    
    //Feather 1
    //std::cout << "Quadrant 1 closest: " << closest[0] << "m -> Intensity: ";
    //printf("Quadrant 1 closest: %.3fm Intensity: ", closest[0]);
    if(closest[0] <= 255 && closest[0] >= 0) {
        int n = closest[0];
        //std::cout << "feather 1: " << n << " " << str_send[n] << std::endl;
        RS232_cputs(cport_nr, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr, str_send[0]);
	}
    
    //Feather 2
    //printf("Quadrant 2 closest: %.3fm Intensity: ", closest[1]);
    if(closest[1] <= 255 && closest[1] >= 0) {
        int n = closest[1] + 256;
        //std::cout << "feather 2: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+256]);
	}
    
    //Feather 3
    //printf("Quadrant 3 closest: %.3fm Intensity: ", closest[2]);
    if(closest[2] <= 255 && closest[2] >= 0) {
        int n = closest[2] + 2*256;
        //std::cout << "feather 3: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(2*256)]);
	}
    
    //Feather 4
    //printf("Quadrant 4 closest: %.3fm Intensity: ", closest[3]);
    if(closest[3] <= 255 && closest[3] >= 0) {
        int n = closest[3] + 3*256;
        //std::cout << "feather 4: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(3*256)]);
	}
    
    //Feather 5
    //printf("Quadrant 5 closest: %.3fm Intensity: ", closest[4]);
    if(closest[4] <= 255 && closest[4] >= 0) {
        int n = closest[4] + 4*256;
        //std::cout << "feather 5: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(4*256)]);
	}
    
    //Feather 6
    //printf("Quadrant 6 closest: %.3fm Intensity: ", closest[5]);
    if(closest[5] <= 255 && closest[5] >= 0) {
        int n = closest[5] + 5*256;
        //std::cout << "feather 6: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(5*256)]);
	}
    
    //Feather 7
    //printf("Quadrant 7 closest: %.3fm Intensity: ", closest[6]);
    if(closest[6] <= 255 && closest[6] >= 0) {
        int n = closest[6] + 6*256;
        //std::cout << "feather 7: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(6*256)]);
	}
    
    //Feather 8
    //printf("Quadrant 8 closest: %.3fm Intensity: ", closest[7]);
    if(closest[7] <= 255 && closest[7] >= 0) {
        int n = closest[7] + 7*256;
        //std::cout << "feather 8: " << n << " " << str_send[n] << std::endl;
		RS232_cputs(cport_nr, str_send[n]);
	}
    else {
        RS232_cputs(cport_nr, str_send[0+(7*256)]);
	}    
    
    return color;
}

float get_depth_scale(rs2::device dev)
{
    // Go over the device's sensors
    for (rs2::sensor& sensor : dev.query_sensors())
    {
        // Check if the sensor if a depth sensor
        if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
        {
            return dpt.get_depth_scale();
        }
    }
    throw std::runtime_error("Device does not have a depth sensor");
}

rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
    //Given a vector of streams, we try to find a depth stream and another stream to align depth with.
    //We prioritize color streams to make the view look better.
    //If color is not available, we take another stream that (other than depth)
    rs2_stream align_to = RS2_STREAM_ANY;
    bool depth_stream_found = false;
    bool color_stream_found = false;
    for (rs2::stream_profile sp : streams)
    {
        rs2_stream profile_stream = sp.stream_type();
        if (profile_stream != RS2_STREAM_DEPTH)
        {
            if (!color_stream_found)         //Prefer color
                align_to = profile_stream;

            if (profile_stream == RS2_STREAM_COLOR)
            {
                color_stream_found = true;
            }
        }
        else
        {
            depth_stream_found = true;
        }
    }

    if(!depth_stream_found)
        throw std::runtime_error("No Depth stream available");

    if (align_to == RS2_STREAM_ANY)
        throw std::runtime_error("No stream found to align with Depth");

    return align_to;
}

bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev)
{
    for (auto&& sp : prev)
    {
        //If previous profile is in current (maybe just added another)
        auto itr = std::find_if(std::begin(current), std::end(current), [&sp](const rs2::stream_profile& current_sp) { return sp.unique_id() == current_sp.unique_id(); });
        if (itr == std::end(current)) //If it previous stream wasn't found in current
        {
            return true;
        }
    }
    return false;
}

// button to "test" the feathers. sets all feather output to max intensity on one press
// and to zero upon following press
void testButton() {
    RS232_cputs(cport_nr, str_send[255]);
    RS232_cputs(cport_nr, str_send[255+256]);
    RS232_cputs(cport_nr, str_send[255+2*256]);
    RS232_cputs(cport_nr, str_send[255+3*256]);
    RS232_cputs(cport_nr, str_send[255+4*256]);
    RS232_cputs(cport_nr, str_send[255+5*256]);
    RS232_cputs(cport_nr, str_send[255+6*256]);
    RS232_cputs(cport_nr, str_send[255+7*256]);
}
