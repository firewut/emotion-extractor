#include <iostream>
#include <stdlib.h>
#include <map>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "classes.hpp"
#include "json.hpp"

#define MAIN_WINDOW_NAME "Main"
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60
#define SUCCESS_DURATION 2      // Duration of emotion for SUCCESS. Seconds
#define FRAME_CHECK_INTERVAL 100 // Milliseconds

Cascade face(
  "face",
  "./haarcascades/haarcascade_frontalface_alt.xml"
);

Cascade smile(
  "smile",
  "./haarcascades/haarcascade_smile.xml"
);

void printProgress(double);
bool cvMatEQ(cv::Mat, cv::Mat);

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
    cv::Mat frame, prev_frame;

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

        printProgress(double(frames_counter/double(total_frames)));
        if(frames_counter == total_frames){
            break;
        }

        // We do not need to process every frame. Only 50 ms boundary
        int time_position_remainder = remainder(double(frames_counter), double(fps));
        if(time_position_remainder%FRAME_CHECK_INTERVAL!=0){
            continue;
        }


        if(cvMatEQ(prev_frame, frame) == true){
            continue;
        }

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
        prev_frame = frame.clone();
    }

    // Normalize emotion frames
    std::cout << std::endl << std::endl << "Normalizing timestamps" << std::endl;
    for(size_t i = 0; i < emotions.size(); i++) {
        emotions[i].normalize_frames(
            SUCCESS_DURATION*fps // Duration of emotion for SUCCESS. Seconds
        );
    }

    std::cout << "Results: " << std::endl;

    std::map<std::string, std::vector< std::vector<std::string> >> emotions_timestamps;
    for(size_t i = 0; i < emotions.size(); i++) {
        emotions_timestamps[emotions[i].title] = emotions[i].get_time_ranges(fps);
    }

    nlohmann::json j_map(emotions_timestamps);
    std::cout << j_map.dump(4) << std::endl;
}

void printProgress (double percentage){
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf ("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush (stdout);
}

bool cvMatEQ(cv::Mat data1, cv::Mat data2){
    bool success = true;
    // check if is multi dimensional
    if(data1.dims > 2 || data2.dims > 2){
        if( data1.dims != data2.dims || data1.type() != data2.type() ){
          return false;
        }
        for(int32_t dim = 0; dim < data1.dims; dim++){
          if(data1.size[dim] != data2.size[dim]){
            return false;
          }
        }
    }else{
        if(data1.size() != data2.size() || data1.channels() != data2.channels() || data1.type() != data2.type()){
          return false;
        }
    }
    int nrOfElements = data1.total()*data1.elemSize1();
    //bytewise comparison of data
    int cnt = 0;
    for(cnt = 0; cnt < nrOfElements && success; cnt++){
        if(data1.data[cnt] != data2.data[cnt]){
          success = false;
        }
    }
    return success;
}
