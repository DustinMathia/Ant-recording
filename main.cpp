//-------repeating timer

#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include "opencv2/opencv.hpp"

void print(const boost::system::error_code& /*e*/, boost::asio::steady_timer* t, int* count, cv::VideoWriter* video, int frame_width, int frame_height)
{
    std::cout << *count << std::endl;
    ++(*count);

    video->release();
    //boost::this_thread::sleep_for(boost::chrono::milliseconds(10));
    video->open(std::to_string(*count) + ".avi", cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frame_width,frame_height));

    t->expires_at(t->expiry() + boost::asio::chrono::seconds(5));
    t->async_wait(boost::bind(print, boost::asio::placeholders::error, t, count, video, frame_width, frame_height));
}

void thread(boost::asio::io_context* io, boost::thread* thread)
{
    io->run();
}

int main()
{
    boost::asio::io_context io;
    cv::VideoCapture cap(0);
    
    // Check if camera opened successfully
    if(!cap.isOpened())
    {
        std::cout << "Error opening video stream" << std::endl;
        return -1;
    }
    
    // Default resolution of the frame is obtained.The default resolution is system dependent.
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    int count = 0;
    
    // Define the codec and create VideoWriter object.The output is stored in '.avi' file.
    cv::VideoWriter video(std::to_string(count) + ".avi", cv::VideoWriter::fourcc('M','J','P','G'),30, cv::Size(frame_width,frame_height));
    
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    t.async_wait(boost::bind(print, boost::asio::placeholders::error, &t, &count, &video, frame_width, frame_height));
    
    boost::thread th(thread, &io, &th);
    //io.run();
    
    std::string i;
    
    while(1)
    {
        cv::Mat frame;
        
        if( !cap.isOpened() )
        {
            cap.open(0);
        }
        
        // Capture frame-by-frame
        cap >> frame;
        
        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        
        // Write the frame into the file
        video.write(frame);
        
        /*std::cin >> i;
        if(!i.empty())
        {
            std::cout << "Final count is " << count << std::endl;
            break;
        }*/
    }
    
    std::cout << "Bruh" << std::endl;
    
    cap.release();
    video.release();
    
    th.detach();
    th.join();
    
    return 0;
}
