#include <iostream>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "classes.hpp"

#define MAIN_WINDOW_NAME "Main"
#define SUCCESS_DURATION 2   // Duration of emotion for SUCCESS. Seconds
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

Cascade face(
  "face",
  "./haarcascades/haarcascade_frontalface_alt.xml"
);

Cascade smile(
  "smile",
  "./haarcascades/haarcascade_smile.xml"
);

void printProgress(double);

int main(int argc, char** argv){
    std::vector<Cascade> classifier_cascades {
        face,
        smile
    };

    std::vector<Emotion> emotions;
    Emotion smiling_face(
        "smile",
        std::vector<Cascade> {
            face,
            smile
        }
    );
    emotions.push_back(smiling_face);

    // Open temporary file
    std::string filename = argv[1];
    cv::VideoCapture capture(filename);
    cv::Mat frame;

    if( !capture.isOpened() ){
        throw "Error when reading file";
    }

    cvNamedWindow(MAIN_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(MAIN_WINDOW_NAME, 100, 0);

    // Meta info
    int fps = (int)capture.get(CV_CAP_PROP_FPS);
    int frames_counter = 0;
    int total_frames = (int)capture.get(CV_CAP_PROP_FRAME_COUNT);

    bool detected_state;

    while(true){
        capture >> frame;
        if(frame.empty()){
            break;
        }
        frames_counter++;

        for(size_t i = 0; i < emotions.size(); i++) {
            cv::Mat emotion_frame;
            bool detected;

            std::tie(detected, emotion_frame) = emotions[i].detect(
                frame
            );

            if(detected){
                // Appeared
                if(!detected_state) {
                    emotions[i].event_at_frame(frames_counter, true);
                }
            }else{
                // Disappeared
                if(detected_state){
                    emotions[i].event_at_frame(frames_counter,false);
                }
            }
            detected_state = detected;
        }

        printProgress(double(frames_counter/double(total_frames)));
    }

    // Normalize emotion frames
    for(size_t i = 0; i < emotions.size(); i++) {
        emotions[i].normalize_frames(
            SUCCESS_DURATION*fps // Duration of emotion for SUCCESS. Seconds
        );
    }

    std::vector< std::string > system_commands;
    // Build commands for `system` call
    for(size_t i = 0; i < emotions.size(); i++) {
        std::vector< std::vector<std::string> > timeranges = emotions[i].get_time_ranges(fps);
        for(int j = 0; j < timeranges.size(); j++) {
            if(timeranges[j].size() == 2){
                std::cout << emotions[i].title << " - " << timeranges[j][0] << " - " << timeranges[j][1] << '\n';
            }
        }
    }
}

void printProgress (double percentage){
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}
