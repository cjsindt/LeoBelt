// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

#include <librealsense2/rs.hpp>
#include "common/example.hpp"
#include <imgui.h>
#include "imgui_impl_glfw.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "rs232.h"

#define BUF_SIZE 123
char str_send[56][BUF_SIZE]; // send data buffer
int cport_nr = 16;

void render_slider(rect location, float& clipping_dist);
void remove_background(rs2::video_frame& other, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist);
float get_depth_scale(rs2::device dev);
rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams);
bool profile_changed(const std::vector<rs2::stream_profile>& current, const std::vector<rs2::stream_profile>& prev);
void printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale);
void drawLines(rs2::video_frame& other_frame, float depth_scale, const rs2::depth_frame& depth_frame);

int main(int argc, char * argv[]) try
{
    int i=1;
    int bdrate = 57600;
    
    char mode[] ={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
    unsigned char str_recv[BUF_SIZE]; // recv data buffer
    strcpy(str_send[0], "01");
    strcpy(str_send[1], "A1");
    strcpy(str_send[2], "B1");
    strcpy(str_send[3], "C1");
    strcpy(str_send[4], "D1");
    strcpy(str_send[5], "E1");
    strcpy(str_send[6], "F1");
    strcpy(str_send[7], "02");
    strcpy(str_send[8], "A2");
    strcpy(str_send[9], "B2");
    strcpy(str_send[10], "C2");
    strcpy(str_send[11], "D2");
    strcpy(str_send[12], "E2");
    strcpy(str_send[13], "F2");
    strcpy(str_send[14], "03");
    strcpy(str_send[15], "A3");
    strcpy(str_send[16], "B3");
    strcpy(str_send[17], "C3");
    strcpy(str_send[18], "D3");
    strcpy(str_send[19], "E3");
    strcpy(str_send[20], "F3");
    strcpy(str_send[21], "04");
    strcpy(str_send[22], "A4");
    strcpy(str_send[23], "B4");
    strcpy(str_send[24], "C4");
    strcpy(str_send[25], "D4");
    strcpy(str_send[26], "E4");
    strcpy(str_send[27], "F4");
    strcpy(str_send[28], "05");
    strcpy(str_send[29], "A5");
    strcpy(str_send[30], "B5");
    strcpy(str_send[31], "C5");
    strcpy(str_send[32], "D5");
    strcpy(str_send[33], "E5");
    strcpy(str_send[34], "F5");
    strcpy(str_send[35], "06");
    strcpy(str_send[36], "A6");
    strcpy(str_send[37], "B6");
    strcpy(str_send[38], "C6");
    strcpy(str_send[39], "D6");
    strcpy(str_send[40], "E6");
    strcpy(str_send[41], "F6");
    strcpy(str_send[42], "07");
    strcpy(str_send[43], "A7");
    strcpy(str_send[44], "B7");
    strcpy(str_send[45], "C7");
    strcpy(str_send[46], "D7");
    strcpy(str_send[47], "E7");
    strcpy(str_send[48], "F7");
    strcpy(str_send[49], "08");
    strcpy(str_send[50], "A8");
    strcpy(str_send[51], "B8");
    strcpy(str_send[52], "C8");
    strcpy(str_send[53], "D8");
    strcpy(str_send[54], "E8");
    strcpy(str_send[55], "F8");
    
    if(RS232_OpenComport(cport_nr, bdrate, mode)) {
        printf("Can not open comport\n");
        return(0);
    }
    
    usleep(2000000); // Waits 2000ms for stable condition
    
    // Create and initialize GUI related objects
    window app(1280, 720, "LEO Belt"); // Simple window handling
    ImGui_ImplGlfw_Init(app, false);      // ImGui library intializition
    rs2::colorizer c;                     // Helper to colorize depth images
    texture renderer;                     // Helper for renderig images

    // Create a pipeline to easily configure and start the camera
    rs2::pipeline pipe;
    //Calling pipeline's start() without any additional parameters will start the first device
    // with its default streams.
    //The start function returns the pipeline profile which the pipeline used to start the device
    rs2::pipeline_profile profile = pipe.start();

    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    float depth_scale = get_depth_scale(profile.get_device());

    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2_stream align_to = find_stream_to_align(profile.get_streams());

    // Create a rs2::align object.
    // rs2::align allows us to perform alignment of depth frames to others frames
    //The "align_to" is the stream type to which we plan to align depth frames.
    rs2::align align(align_to);

    // Define a variable for controlling the distance to clip
    float depth_clipping_distance = 1.f;

    while (app) // Application still alive?
    {
        // Using the align object, we block the application until a frameset is available
        rs2::frameset frameset = pipe.wait_for_frames();

        // rs2::pipeline::wait_for_frames() can replace the device it uses in case of device error or disconnection.
        // Since rs2::align is aligning depth to some other stream, we need to make sure that the stream was not changed
        //  after the call to wait_for_frames();
        if (profile_changed(pipe.get_active_profile().get_streams(), profile.get_streams()))
        {
            //If the profile was changed, update the align object, and also get the new device's depth scale
            profile = pipe.get_active_profile();
            align_to = find_stream_to_align(profile.get_streams());
            align = rs2::align(align_to);
            depth_scale = get_depth_scale(profile.get_device());
        }

        //Get processed aligned frame
        auto processed = align.process(frameset);

        // Trying to get both other and aligned depth frames
        rs2::video_frame other_frame = processed.first(align_to);
        rs2::depth_frame aligned_depth_frame = processed.get_depth_frame();

        //If one of them is unavailable, continue iteration
        if (!aligned_depth_frame || !other_frame)
        {
            continue;
        }
        // Passing both frames to remove_background so it will "strip" the background
        // NOTE: in this example, we alter the buffer of the other frame, instead of copying it and altering the copy
        //       This behavior is not recommended in real application since the other frame could be used elsewhere
        //remove_background(other_frame, aligned_depth_frame, depth_scale, depth_clipping_distance);
  


        // Taking dimensions of the window for rendering purposes
        float w = static_cast<float>(app.width());
        float h = static_cast<float>(app.height());

        

        // At this point, "other_frame" is an altered frame, stripped form its background
        // Calculating the position to place the frame in the window
        rect altered_other_frame_rect{ 0, 0, w, h };
        altered_other_frame_rect = altered_other_frame_rect.adjust_ratio({ static_cast<float>(other_frame.get_width()),static_cast<float>(other_frame.get_height()) });

        // Render aligned image
        renderer.render(other_frame, altered_other_frame_rect);

        // The example also renders the depth frame, as a picture-in-picture
        // Calculating the position to place the depth frame in the window
        //rect pip_stream{ 0, 0, w / 5, h / 5 };
        //pip_stream = pip_stream.adjust_ratio({ static_cast<float>(aligned_depth_frame.get_width()),static_cast<float>(aligned_depth_frame.get_height()) });
        //pip_stream.x = altered_other_frame_rect.x + altered_other_frame_rect.w - pip_stream.w - (std::max(w, h) / 25);
        //pip_stream.y = altered_other_frame_rect.y + (std::max(w, h) / 25);

        // Render depth (as picture in pipcture)
        //renderer.upload(c.process(aligned_depth_frame));
        //renderer.show(pip_stream);

        // Using ImGui library to provide a slide controller to select the depth clipping distance
        ImGui_ImplGlfw_NewFrame(1);
        //render_slider({ 5.f, 0, w, h }, depth_clipping_distance);
        ImGui::Render();
        
        /*
        RS232_cputs(cport_nr, str_send[i]);
        //printf("Sent to Arduino: '%s'\n", str_send[i]);
        //usleep(1000);
        int n = RS232_PollComport(cport_nr, str_recv, (int)BUF_SIZE);
        if(n > 0) {
            str_recv[n] = 0;
            //printf("Received %i bytes: '%s'\n", n, (char *)str_recv);
        }
        i++;
        i %= 7;
        usleep(5000000);
        */ 

        // Print pixel depth
        printPixelDepth(other_frame, aligned_depth_frame, depth_scale);
        //drawLines(other_frame, depth_scale, aligned_depth_frame);


    }
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

void render_slider(rect location, float& clipping_dist)
{
    // Some trickery to display the control nicely
    static const int flags = ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove;
    const int pixels_to_buttom_of_stream_text = 25;
    const float slider_window_width = 30;

    ImGui::SetNextWindowPos({ location.x, location.y + pixels_to_buttom_of_stream_text });
    ImGui::SetNextWindowSize({ slider_window_width + 20, location.h - (pixels_to_buttom_of_stream_text * 2) });

    ImGui::Begin("Shapes", nullptr, flags);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(215.f / 255, 215.0f / 255, 215.0f / 255));
    ImGui::End();

    /*
    //Render the vertical slider
    ImGui::Begin("slider", nullptr, flags);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(215.f / 255, 215.0f / 255, 215.0f / 255));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImColor(215.f / 255, 215.0f / 255, 215.0f / 255));
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImColor(215.f / 255, 215.0f / 255, 215.0f / 255));
    auto slider_size = ImVec2(slider_window_width / 2, location.h - (pixels_to_buttom_of_stream_text * 2) - 20);
    ImGui::VSliderFloat("", slider_size, &clipping_dist, 0.0f, 6.0f, "", 1.0f, true);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Depth Clipping Distance: %.3f", clipping_dist);
    ImGui::PopStyleColor(3);

    //Display bars next to slider
    float bars_dist = (slider_size.y / 6.0f);
    for (int i = 0; i <= 6; i++)
    {
        ImGui::SetCursorPos({ slider_size.x, i * bars_dist });
        std::string bar_text = "- " + std::to_string(6-i) + "m";
        ImGui::Text("%s", bar_text.c_str());
    }
    ImGui::End();
    */
}

void drawLines(rs2::video_frame& other_frame, float depth_scale, const rs2::depth_frame& depth_frame) {
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));
    int other_bpp = other_frame.get_bytes_per_pixel();

    int width = other_frame.get_width();
    int height = other_frame.get_height();

    int y = floor(height / 3);
    //for (y = floor(height / 3); y < (floor(height / 3) + 3); y++) {

    auto depth_pixel_index = y * width;
    for (int l =  0; l < height; l ++) {
        for (int x = 0; x < width; x++, ++depth_pixel_index) {
            auto offset = depth_pixel_index * other_bpp;

            // Set pixel to "background" color (0x999999)
            std::memset(&p_other_frame[offset], 0x99, other_bpp);
        }
    }
    /*
    y *= 2;
    depth_pixel_index = y * width;
    for (int x = 0; x < width; x++, ++depth_pixel_index) {
        auto offset = depth_pixel_index * other_bpp;

        // Set pixel to "background" color (0x999999)
        std::memset(&p_other_frame[offset], 990000, other_bpp);
    }

    depth_pixel_index = floor(width / 3);
    for (int y2 = 0; y2 < height; y2++, ++depth_pixel_index) {
        auto offset = depth_pixel_index * other_bpp;

        std::memset(&p_other_frame[offset], 990000, other_bpp);
    }
    * */

    /*
    int x = floor(width / 3);
    depth_pixel_index = x * height;
    for (int y2 = 0; y2 < height; y2++, ++depth_pixel_index) {
        auto offset = depth_pixel_index * other_bpp;

        std::memset(&p_other_frame[offset], 990000, other_bpp);
    }
    */
    //}

    /*
    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 50; y < 55; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Calculate the offset in other frame's buffer to current pixel
            auto offset = depth_pixel_index * other_bpp;

            // Set pixel to "background" color (0x999999)
            std::memset(&p_other_frame[offset], 0x99, other_bpp);
        }
    }
    */
}

void remove_background(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale, float clipping_dist)
{
    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());
    uint8_t* p_other_frame = reinterpret_cast<uint8_t*>(const_cast<void*>(other_frame.get_data()));

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    #pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];

            // Check if the depth value is invalid (<=0) or greater than the threashold
            if (pixels_distance <= 0.f || pixels_distance > clipping_dist)
            {
                // Calculate the offset in other frame's buffer to current pixel
                auto offset = depth_pixel_index * other_bpp;

                // Set pixel to "background" color (0x999999)
                std::memset(&p_other_frame[offset], 0x99, other_bpp);

            }
        }
    }
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

void printPixelDepth(rs2::video_frame& other_frame, const rs2::depth_frame& depth_frame, float depth_scale) {

    const uint16_t* p_depth_frame = reinterpret_cast<const uint16_t*>(depth_frame.get_data());

    int width = other_frame.get_width();
    int height = other_frame.get_height();
    int other_bpp = other_frame.get_bytes_per_pixel();

    int two_d[2][2];
    float closest[8] = {10,10,10,10,10,10,10,10};

#pragma omp parallel for schedule(dynamic) //Using OpenMP to try to parallelise the loop
    for (int y = 0; y < height; y++)
    {
        auto depth_pixel_index = y * width;
        for (int x = 0; x < width; x++, ++depth_pixel_index)
        {
            // Get the depth value of the current pixel
            auto pixels_distance = depth_scale * p_depth_frame[depth_pixel_index];
            
            // Working code for two feathers and the screen split into 2 quadrants
            /*
            if (x < width / 2) { // left half of screen
                if (pixels_distance < closest[0] && pixels_distance > .001){
                        closest[0] = pixels_distance;
                }
            } else { // right half of screen
                if (pixels_distance < closest[1] && pixels_distance > .001){
                        closest[1] = pixels_distance;
                }
            }
            */
            
            /* Working code for 4 quadrants
            // Code for 4 qudrants
            if (x < width / 2 && y < height/ 2) { // Top-left quadrant
                if (pixels_distance < closest[0] && pixels_distance > .001){
                    closest[0] = pixels_distance;
                }
            } else if (x > width / 2 && y < height / 2) { // Top-right quadrant
                if (pixels_distance < closest[1] && pixels_distance > .001){
                    closest[1] = pixels_distance;
                }
            } else if (x < width / 2 && y > height / 2) { // Bottom-left quadrant
                if (pixels_distance < closest[2] && pixels_distance > .001){
                    closest[2] = pixels_distance;
                }
            } else if ( x > width / 2 && y > height / 2) { // Bottom-right quadrant
                if (pixels_distance < closest[3] && pixels_distance > .001){
                    closest[3] = pixels_distance;
                }
            }
            */
            
            // Attempting 8 feathers
            if ((x < width / 3) && (y < height / 3)) { // Top-left quadrant
                if (pixels_distance < closest[0] && pixels_distance > .001){
                    closest[0] = pixels_distance;
                }
            } else if ((x > width / 3) && (x < 2*(width / 3)) && (y < height / 3)) { // Top-middle quadrant
                if (pixels_distance < closest[1] && pixels_distance > .001){
                    closest[1] = pixels_distance;
                }
            } else if ((x > 2*(width / 3)) && (y < height / 3)) { // Top-right quadrant
                if (pixels_distance < closest[2] && pixels_distance > .001){
                    closest[2] = pixels_distance;
                }
            } else if ((x < width / 3) && (y > height / 3) && (y < 2*(height / 3))) { // Middle-left quadrant
                if (pixels_distance < closest[3] && pixels_distance > .001){
                    closest[3] = pixels_distance;
                }
            } else if ((x > width / 3) && (x < 2*(width / 3)) && (y > height / 3) && (y < 2*(height / 3))) { // Middle-middle quadrant
                if (pixels_distance < closest[4] && pixels_distance > .001){
                    closest[4] = pixels_distance;
                }
            } else if ((x > 2*(width / 3)) && (y > (height / 3)) && (y < (2*(height / 3)))) { // Middle-right quadrant
                if (pixels_distance < closest[5] && pixels_distance > .001){
                    closest[5] = pixels_distance;
                }
            } else if ((x < width / 2) && (y > 2*(height / 3))) { // Bottom-left quadrant
                if (pixels_distance < closest[6] && pixels_distance > .001){
                    closest[6] = pixels_distance;
                }
            } else if ((x > width / 2) && (y > 2*(height / 3))) { // Bottom-right quadrant
                if (pixels_distance < closest[7] && pixels_distance > .001){
                    closest[7] = pixels_distance;
                }
            }
            
            

            //if (pixels_distance > furthest) { furthest = pixels_distance; }
            //if ((pixels_distance < closest) && (pixels_distance > .1)) { closest = pixels_distance; }

            
            //std::cout << "Depth at index (height, width) = [" << y << "," << x << "] = " << pixels_distance << std::endl;
            //std::cout << "Depth pixel index = " << depth_pixel_index << std::endl;
            
            /*

            if ((x <= 640) && (y <= 240)) {
                std::cout << "Bottom-left qudrant" << std::endl;
            }
            else if ((x > 640 && x <= 1280) && (y < 240)) {
                std::cout << "Bottom-right quadrant" << std::endl;
            }
            else if ((x < 640) && (y > 240 && y <= 480)) {
                std::cout << "Middle-left quadrant" << std::endl;
            }
            else if ((x > 640 && x <= 1280) && (y > 240 && y <= 480)) {
                std::cout << "Middle-right quadrant" << std::endl;
            }
            else if ((x <= 426) && (y > 480 && y <= 720)) {
                std::cout << "Top-left quadrant" << std::endl;
            }
            else if ((x > 427 && x <= 853) && (y > 481 && y <= 720)) {
                std::cout << "Top-middle quadrant" << std::endl;
            }
            else if ((x > 854 && x <= 1280) && (y > 480 && y <= 720)) {
                std::cout << "Top-right quadrant" << std::endl;
            }
            */
        }
        //std::cout << "Height: " << height << "m. Width: " << width << "m." << std::endl;
    }
    //Feather 1
    //std::cout << "Quadrant 1 closest: " << closest[0] << "m -> Intensity: ";
    printf("Quadrant 1 closest: %fm Intensity: ", closest[0]);
    if(closest[0] < .33){
        //RS232_cputs(cport_nr, str_send[6]);
        std::cout << "6/6" << std::endl;
    } else if(closest[0] < .66){
        //RS232_cputs(cport_nr, str_send[5]);
        std::cout << "5/6" << std::endl;
    } else if(closest[0] < .99){
        //RS232_cputs(cport_nr, str_send[4]);
        std::cout << "4/6" << std::endl;
    } else if(closest[0] < 1.33){
        //RS232_cputs(cport_nr, str_send[3]);
        std::cout << "3/6" << std::endl;
    } else if(closest[0] < 1.66){
        //RS232_cputs(cport_nr, str_send[2]);
        std::cout << "2/6" << std::endl;
    } else if(closest[0] < 1.99){
        //RS232_cputs(cport_nr, str_send[1]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[0]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 2
    std::cout << "Quadrant 2 closest: " << closest[1] << "m -> Intensity: ";
    if(closest[1] < .33){
        //RS232_cputs(cport_nr, str_send[13]);
        std::cout << "6/6\r" << std::endl;
    } else if(closest[1] < .66){
        //RS232_cputs(cport_nr, str_send[12]);
        std::cout << "5/6\r" << std::endl;
    } else if(closest[1] < .99){
        //S232_cputs(cport_nr, str_send[11]);
        std::cout << "4/6\r" << std::endl;
    } else if(closest[1] < 1.33){
        //RS232_cputs(cport_nr, str_send[10]);
        std::cout << "3/6\r" << std::endl;
    } else if(closest[1] < 1.66){
        //RS232_cputs(cport_nr, str_send[9]);
        std::cout << "2/6\r" << std::endl;
    } else if(closest[1] < 1.99){
        //RS232_cputs(cport_nr, str_send[8]);
        std::cout << "1/6\r" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[7]);
        std::cout << "0/6\r" << std::endl;
    }
    
    //Feather 3
    std::cout << "Quadrant 3 closest: " << closest[2] << "m -> Intensity: ";
    if(closest[2] < .33){
        //RS232_cputs(cport_nr, str_send[20]);
        std::cout << "6/6" << std::endl;
    } else if(closest[2] < .66){
        //RS232_cputs(cport_nr, str_send[19]);
        std::cout << "5/6" << std::endl;
    } else if(closest[2] < .99){
        //RS232_cputs(cport_nr, str_send[18]);
        std::cout << "4/6" << std::endl;
    } else if(closest[2] < 1.33){
        //RS232_cputs(cport_nr, str_send[17]);
        std::cout << "3/6" << std::endl;
    } else if(closest[2] < 1.66){
       //RS232_cputs(cport_nr, str_send[16]);
        std::cout << "2/6" << std::endl;
    } else if(closest[2] < 1.99){
        //RS232_cputs(cport_nr, str_send[15]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[14]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 4
    std::cout << "Quadrant 4 closest: " << closest[3] << "m -> Intensity: ";
    if(closest[3] < .33){
        //RS232_cputs(cport_nr, str_send[27]);
        std::cout << "6/6" << std::endl;
    } else if(closest[3] < .66){
        //RS232_cputs(cport_nr, str_send[26]);
        std::cout << "5/6" << std::endl;
    } else if(closest[3] < .99){
        //RS232_cputs(cport_nr, str_send[25]);
        std::cout << "4/6" << std::endl;
    } else if(closest[3] < 1.33){
        //RS232_cputs(cport_nr, str_send[24]);
        std::cout << "3/6" << std::endl;
    } else if(closest[3] < 1.66){
       //RS232_cputs(cport_nr, str_send[23]);
        std::cout << "2/6" << std::endl;
    } else if(closest[3] < 1.99){
        //RS232_cputs(cport_nr, str_send[22]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[21]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 5
    std::cout << "Quadrant 5 closest: " << closest[4] << "m -> Intensity: ";
    if(closest[4] < .33){
        //RS232_cputs(cport_nr, str_send[34]);
        std::cout << "6/6" << std::endl;
    } else if(closest[4] < .66){
        //RS232_cputs(cport_nr, str_send[33]);
        std::cout << "5/6" << std::endl;
    } else if(closest[4] < .99){
        //RS232_cputs(cport_nr, str_send[32]);
        std::cout << "4/6" << std::endl;
    } else if(closest[4] < 1.33){
        //RS232_cputs(cport_nr, str_send[31]);
        std::cout << "3/6" << std::endl;
    } else if(closest[4] < 1.66){
       //RS232_cputs(cport_nr, str_send[30]);
        std::cout << "2/6" << std::endl;
    } else if(closest[4] < 1.99){
        //RS232_cputs(cport_nr, str_send[29]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[28]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 6
    std::cout << "Quadrant 6 closest: " << closest[5] << "m -> Intensity: ";
    if(closest[5] < .33){
        //RS232_cputs(cport_nr, str_send[41]);
        std::cout << "6/6" << std::endl;
    } else if(closest[5] < .66){
        //RS232_cputs(cport_nr, str_send[40]);
        std::cout << "5/6" << std::endl;
    } else if(closest[5] < .99){
        //RS232_cputs(cport_nr, str_send[39]);
        std::cout << "4/6" << std::endl;
    } else if(closest[5] < 1.33){
        //RS232_cputs(cport_nr, str_send[38]);
        std::cout << "3/6" << std::endl;
    } else if(closest[5] < 1.66){
       //RS232_cputs(cport_nr, str_send[37]);
        std::cout << "2/6" << std::endl;
    } else if(closest[5] < 1.99){
        //RS232_cputs(cport_nr, str_send[36]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[35]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 7
    std::cout << "Quadrant 7 closest: " << closest[6] << "m -> Intensity: ";
    if(closest[6] < .33){
        //RS232_cputs(cport_nr, str_send[48]);
        std::cout << "6/6" << std::endl;
    } else if(closest[6] < .66){
        //RS232_cputs(cport_nr, str_send[47]);
        std::cout << "5/6" << std::endl;
    } else if(closest[6] < .99){
        //RS232_cputs(cport_nr, str_send[46]);
        std::cout << "4/6" << std::endl;
    } else if(closest[6] < 1.33){
        //RS232_cputs(cport_nr, str_send[45]);
        std::cout << "3/6" << std::endl;
    } else if(closest[6] < 1.66){
       //RS232_cputs(cport_nr, str_send[44]);
        std::cout << "2/6" << std::endl;
    } else if(closest[6] < 1.99){
        //RS232_cputs(cport_nr, str_send[43]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[42]);
        std::cout << "0/6" << std::endl;
    }
    
    //Feather 8
    std::cout << "Quadrant 8 closest: " << closest[7] << "m -> Intensity: ";
    if(closest[7] < .33){
        //RS232_cputs(cport_nr, str_send[55]);
        std::cout << "6/6" << std::endl;
    } else if(closest[7] < .66){
        //RS232_cputs(cport_nr, str_send[54]);
        std::cout << "5/6" << std::endl;
    } else if(closest[7] < .99){
        //RS232_cputs(cport_nr, str_send[53]);
        std::cout << "4/6" << std::endl;
    } else if(closest[7] < 1.33){
        //RS232_cputs(cport_nr, str_send[52]);
        std::cout << "3/6" << std::endl;
    } else if(closest[7] < 1.66){
       //RS232_cputs(cport_nr, str_send[51]);
        std::cout << "2/6" << std::endl;
    } else if(closest[7] < 1.99){
        //RS232_cputs(cport_nr, str_send[50]);
        std::cout << "1/6" << std::endl;
    } else {
        //RS232_cputs(cport_nr, str_send[49]);
        std::cout << "0/6" << std::endl;
    }
    
    usleep(100000);
    //std::cout << "Furthest: " << furthest << "m. Closest: " << closest << "m." << std::endl;
}
