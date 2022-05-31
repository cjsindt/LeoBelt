// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp>
#include "third-party/imgui/imgui.h" // <imgui.h>
#include "third-party/imgui/imgui_impl_glfw.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "rs232.h"
#include <chrono>
#include <ctime>

#define PI 3.14159265

#define BUF_SIZE 123
char str_send[2048][BUF_SIZE]; // send data buffer
int cport_nr1_4 = 16;
int cport_nr5_8 = 17;
float h_height = 1.7;
float h_width = .5;
int getFrame = 0;

float get_depth_scale(rs2::device dev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
int* printPixelDepth(const rs2::depth_frame& depth_frame, float depth_scale);
void silenceAllFeathers();
void testButton();
float* findMags(float y_angle, float z_angle, float pixel_dist);

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

    if(RS232_OpenComport(cport_nr1_4, bdrate, mode)) {
        printf("Can not open comport\n");
        //return(0);
    }
    if(RS232_OpenComport(cport_nr5_8, bdrate, mode)) {
        printf("Can not open comport\n");
        //return(0);
    }

    // Create a pipeline to easily configure and start the camera
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH, -1, 640, 480, RS2_FORMAT_ANY, 60);
    rs2::pipeline pipe;

    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start(cfg);

    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    rs2::frameset frameset;
    int* c;

    auto start = std::chrono::high_resolution_clock::now();
    int duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-start).count();
    std::time_t start_time = std::chrono::system_clock::to_time_t(start);
    int minute = 0;

    std::ofstream out("/home/pi/leo_belt/log.txt", std::ios::app);
    std::cout.rdbuf(out.rdbuf());

    std::cout << "\nProgram start: " << std::ctime(&start_time);

    while (true) // Application still alive?
    {
        try {
            // Using the align object, we block the application until a frameset is available
            frameset = pipe.wait_for_frames(34);
        }
        catch (const std::exception& e) {
            continue;
        }

        if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = pipe.get_active_profile();
            depth_scale = get_depth_scale(profile.get_device());
        }

        // Trying to get both other and aligned depth frames
        rs2::depth_frame aligned_depth_frame = frameset.get_depth_frame();

        //If one of them is unavailable, continue iteration
        if (!aligned_depth_frame)
        {
            continue;
        }

        // Obtaining pixel depth
        c = printPixelDepth(aligned_depth_frame, depth_scale);

        getFrame++;
        duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()-start).count();

        if (duration%60 == 0) {
            if (getFrame > 70) {
                minute++;
                std::cout << "minute: " << minute << " fps: " << getFrame / 60 << std::endl;
                getFrame = 0;
            }
        }
    }
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
    RS232_cputs(cport_nr1_4, str_send[0]);
    RS232_cputs(cport_nr1_4, str_send[256]);
    RS232_cputs(cport_nr1_4, str_send[2*256]);
    RS232_cputs(cport_nr1_4, str_send[3*256]);
    RS232_cputs(cport_nr5_8, str_send[4*256]);
    RS232_cputs(cport_nr5_8, str_send[5*256]);
    RS232_cputs(cport_nr5_8, str_send[6*256]);
    RS232_cputs(cport_nr5_8, str_send[7*256]);
}

int* printPixelDepth(const rs2::depth_frame& depth_frame, float depth_scale) {

    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());

    int width = 640;
    int height = 480;

    float closest[8] = {10,10,10,10,10,10,10,10};
    static int color[16] = {};

    float y_angle = 0;
    float z_angle = 0;
    float x_mag, z_mag;
    float* mags = {};
    float closest_pix = 10;
    auto depth_pixel_index = 0;
    float pixels_distance = 0;

#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int j = 0; j < height; j=j+4) {
        for (int i = 0; i < width; i=i+4) {

            closest_pix = 10;

            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                    depth_pixel_index = j*width + i + y*width + x;
                    pixels_distance = depth_scale*p_depth_frame[depth_pixel_index];

                    if (pixels_distance < closest_pix && pixels_distance != 0) {
                        closest_pix = pixels_distance;
                        y_angle = 43.5 - (i+x)*(float)(43.5/424);
                        z_angle = 29.0 - (j+y)*(float)(29.0/240);

                        if (y_angle < 0) { y_angle = -y_angle; }
                        if (z_angle < 0) { z_angle = -z_angle; }
                    }

                }
            }

            mags = findMags(y_angle, z_angle, closest_pix);
            x_mag = *(mags+0);
            z_mag = *(mags+1);

            if (x_mag > h_width/2 && z_mag > h_height/2) { pixels_distance = sqrt(pow(closest_pix,2) + pow(x_mag-h_width/2,2) + pow(z_mag-h_height/2,2)); }
            else if (x_mag > h_width/2) { pixels_distance = sqrt(pow(closest_pix,2) + pow(x_mag-h_width/2,2)); }
            else if (z_mag > h_height/2) { pixels_distance = sqrt(pow(closest_pix,2) + pow(z_mag-h_height/2,2)); }
            else { pixels_distance = closest_pix; }

            if (i<width/3 && j<height/3) { // Top-left quadrant -- Feather 1
                if (pixels_distance < closest[0] && pixels_distance > .001){
                    closest[0] = pixels_distance;
                }
            } else if (i<2*width/3 && j<height/3) { // Top-middle quadrant -- Feather 2
                if (pixels_distance < closest[1] && pixels_distance > .001){
                    closest[1] = pixels_distance;
                }
            } else if (j<height/3) { // Top-right quadrant -- Feather 3
                if (pixels_distance < closest[2] && pixels_distance > .001){
                    closest[2] = pixels_distance;
                }
            } else if (i<width/3 && j<2*height/3) { // Middle-left quadrant -- Feather 4
                if (pixels_distance < closest[3] && pixels_distance > .001){
                    closest[3] = pixels_distance;
                }
            } else if (i<2*width/3 && j<2*height/3) { // Middle-middle quadrant -- Feather 5
                if (pixels_distance < closest[4] && pixels_distance > .001){
                    closest[4] = pixels_distance;
                }
            } else if (j<2*height/3) { // Middle-right quadrant -- Feather 6
                if (pixels_distance < closest[5] && pixels_distance > .001){
                    closest[5] = pixels_distance;
                }
            } else if (i<width/2 && j>2*height/3) { // Bottom-left quadrant -- Feather 7
                if (pixels_distance < closest[6] && pixels_distance > .001){
                    closest[6] = pixels_distance;
                }
            } else if (j>2*height/3) { // Bottom-right quadrant -- Feather 8
                if (pixels_distance < closest[7] && pixels_distance > .001){
                    closest[7] = pixels_distance;
                }
            }
        }
    }

    // converting physical distance to 0-255 scale
    for(int i=0; i<=7; i++) {
        closest[i] = 255 + ((0-255.0)*(closest[i]-.001)/(1.413-.001));
        closest[i] = trunc(closest[i]);
    }

    // Obtaining (R)G(B) color values for each quadrant in heat map
    for(int i=0; i<=7; i++) {
        if(closest[i]<=255 && closest[i]>=0) {
            color[i] = closest[i];
            color[i+8] = 255 - closest[i];
        }
            // Anything greater than 2m away will remain blue on map
        else {
            color[i] = 0;
            color[i+8] = 255;
        }
    }

    //Feather 1
    if(closest[0] <= 255 && closest[0] >= 0) {
        int n = closest[0];
        RS232_cputs(cport_nr1_4, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr1_4, str_send[0]);
    }

    //Feather 2
    if(closest[1] <= 255 && closest[1] >= 0) {
        int n = closest[1] + 256;
        RS232_cputs(cport_nr1_4, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr1_4, str_send[0+256]);
    }

    //Feather 3
    if(closest[2] <= 255 && closest[2] >= 0) {
        int n = closest[2] + 2*256;
        RS232_cputs(cport_nr1_4, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr1_4, str_send[0+(2*256)]);
    }

    //Feather 4
    if(closest[3] <= 255 && closest[3] >= 0) {
        int n = closest[3] + 3*256;
        RS232_cputs(cport_nr1_4, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr1_4, str_send[0+(3*256)]);
    }

    //Feather 5
    if(closest[4] <= 255 && closest[4] >= 0) {
        int n = closest[4] + 4*256;
        RS232_cputs(cport_nr5_8, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr5_8, str_send[0+(4*256)]);
    }

    //Feather 6
    if(closest[5] <= 255 && closest[5] >= 0) {
        int n = closest[5] + 5*256;
        RS232_cputs(cport_nr5_8, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr5_8, str_send[0+(5*256)]);
    }

    //Feather 7
    if(closest[6] <= 255 && closest[6] >= 0) {
        int n = closest[6] + 6*256;
        RS232_cputs(cport_nr5_8, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr5_8, str_send[0+(6*256)]);
    }

    //Feather 8
    if(closest[7] <= 255 && closest[7] >= 0) {
        int n = closest[7] + 7*256;
        RS232_cputs(cport_nr5_8, str_send[n]);
    }
    else {
        RS232_cputs(cport_nr5_8, str_send[0+(7*256)]);
    }

    return color;
}

float* findMags(float y_angle, float z_angle, float pixel_dist) {
    float x, z;
    static float a[2] = {};

    z = pixel_dist*tan(z_angle*PI/180);
    x = pixel_dist*tan(y_angle*PI/180);

    a[0] = x;
    a[1] = z;

    return a;
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
    RS232_cputs(cport_nr1_4, str_send[255]);
    RS232_cputs(cport_nr1_4, str_send[255+256]);
    RS232_cputs(cport_nr1_4, str_send[255+2*256]);
    RS232_cputs(cport_nr1_4, str_send[255+3*256]);
    RS232_cputs(cport_nr5_8, str_send[255+4*256]);
    RS232_cputs(cport_nr5_8, str_send[255+5*256]);
    RS232_cputs(cport_nr5_8, str_send[255+6*256]);
    RS232_cputs(cport_nr5_8, str_send[255+7*256]);
}
