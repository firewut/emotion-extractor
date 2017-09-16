#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "classes.hpp"

// CASCADE
Cascade::Cascade(std::string cascade_title, std::string cascade_xml_path){
    title = cascade_title;
    classifierFile = cascade_xml_path;

    if( !cascadeClassifier.load( classifierFile ) ){
        std::cout << "Error loading" << classifierFile << "cascade" << std::endl;
        throw "Cascade load failed";
    }
}

std::tuple<std::vector<cv::Rect>, cv::Mat> Cascade::detect(cv::Mat frame){
    std::vector<cv::Rect> detections;

    cascadeClassifier.detectMultiScale(
        frame,
        detections,
        1.1,
        2,
        0 | CV_HAAR_SCALE_IMAGE,
        cv::Size(30, 30)
    );

    // Apply mask to all detected regions
    cv::Mat mask = cv::Mat::zeros(
        frame.size(),
        frame.type()
    );
    cv::Mat dstImage = cv::Mat::zeros(
        frame.size(),
        frame.type()
    );

    // Draw rectangles at mask
    for(size_t d_i = 0; d_i < detections.size(); d_i++) {
        cv::rectangle(
            mask,
            cv::Point(
                detections[d_i].x,
                detections[d_i].y
            ),
            cv::Point(
                detections[d_i].x + detections[d_i].width,
                detections[d_i].y + detections[d_i].height
            ),
            cv::Scalar( 255, 0, 0 ),
            CV_FILLED,
            8,
            0
        );
    }
    frame.copyTo(dstImage, mask);

    return std::make_tuple(detections, dstImage);
}

// EMOTION
Emotion::Emotion(std::string emotion_title, std::vector<Cascade> emotion_cascades){
    title = emotion_title;
    cascades = emotion_cascades;
}

void Emotion::event_at_frame(int frame_number, bool appears){
    if(appears){
        appear_frames.push_back(frame_number);
    }else{
        disappear_frames.push_back(frame_number);
    }
}

void Emotion::normalize_frames(int success_frames_count){
    /*
      appears:
          1  23 25 42 47 51    67 73 79 83 85 94  104 1000 1030
      disappears:
          24 27 43 48 52 69    74 80 84 92 95 105 120 1003 1500
    */

    /* Check appears ranges. When `success_frames_count` is 30
        appears:
            1  23 25 42 47 51    67
        converts to
            1 67
        disappears:
            24 27 43 48 52 69    74 80 84 92 95 105 120
        converts to
            52 120
    */
    int border_frame_position = 0;
    for(int i = 1; i < appear_frames.size(); i++){
        int current_frame = appear_frames[i];
        int frame_diff = current_frame - appear_frames[border_frame_position];

        if(frame_diff < success_frames_count){
            appear_frames[i] = -1;
            for(int j = border_frame_position; j < i; j++){
                disappear_frames[j] = -1;
            }
        }else{
            border_frame_position = i;
        }
    }

    appear_frames.erase(
        std::remove(appear_frames.begin(), appear_frames.end(), -1),
        appear_frames.end()
    );
    disappear_frames.erase(
        std::remove(disappear_frames.begin(), disappear_frames.end(), -1),
        disappear_frames.end()
    );

    join_frames(success_frames_count);
}

void Emotion::join_frames(int success_frames_count){
    /* Check appears/disappears ranges
        appears:
            1 67 > 1
        disappears:
            52 120 > 120

        because: range(1, 52) intersects(67, 120) in casce
          when `success_frames_count` is 30
    */
    bool join_triggered = false;

    int border_frame_position = 0;
    for(int i = 1; i < appear_frames.size(); i++){
        int current_frame = appear_frames[i]; // 67
        int frame_diff = current_frame - disappear_frames[border_frame_position]; // 67 - 52

        if(frame_diff < success_frames_count){
            appear_frames[i] = -1; // 67 -> -1
            disappear_frames[border_frame_position] = -1;

            join_triggered = true;
        }else{
            border_frame_position = i;
        }
    }

    if(join_triggered){
        appear_frames.erase(
            std::remove(appear_frames.begin(), appear_frames.end(), -1),
            appear_frames.end()
        );
        disappear_frames.erase(
            std::remove(disappear_frames.begin(), disappear_frames.end(), -1),
            disappear_frames.end()
        );
        join_frames(success_frames_count);
    }
}

std::vector< std::vector<std::string> > Emotion::get_time_ranges(int fps){
    std::vector< std::vector<std::string> > result;

    if(appear_frames.size() == disappear_frames.size()){
        for(int i = 0; i < appear_frames.size(); i++){
            char appear_timestamp[10], disappear_timestamp[10];
            std::vector<std::string> frame_ranges;
            int total_seconds = 0;
            int seconds = 0;
            int hours = 0;
            int minutes = 0;

            // Appear
            total_seconds = appear_frames[i] / fps;
            hours = (total_seconds / 60 / 60) % 24;
            minutes = (total_seconds / 60) % 60;
            seconds = total_seconds % 60;
            sprintf(
                appear_timestamp,
                "%02d:%02d:%02d",
                hours,
                minutes,
                seconds
            );
            frame_ranges.push_back(appear_timestamp);

            // Disappear
            total_seconds = disappear_frames[i] / fps;
            hours = (total_seconds / 60 / 60) % 24;
            minutes = (total_seconds / 60) % 60;
            seconds = total_seconds % 60;
            sprintf(
                disappear_timestamp,
                "%02d:%02d:%02d",
                hours,
                minutes,
                seconds
            );
            frame_ranges.push_back(disappear_timestamp);


            result.push_back(frame_ranges);
        }
    }

    return result;
}


std::tuple<bool, cv::Mat> Emotion::detect(cv::Mat frame){
    std::vector<cv::Rect> detections;
    bool last_classifier_detection_success;

    // Possible
    cv::Mat frame_gray, frame_debug;
    cv::cvtColor(frame, frame_gray, CV_BGR2GRAY);

    for(size_t i = 0; i < cascades.size(); i++){
        std::tie(detections, frame_gray) = cascades[i].detect(
            frame_gray
        );
        if(i == cascades.size()-1){
            last_classifier_detection_success = detections.size() > 0;
        }
    }

    // for(size_t i = 0; i < overall_detections.size(); i++) {
    //     cv::rectangle(
    //         frame,
    //         cv::Point(
    //           overall_detections[i].x,
    //           overall_detections[i].y
    //         ),
    //         cv::Point(
    //           overall_detections[i].x + overall_detections[i].width,
    //           overall_detections[i].y + overall_detections[i].height
    //         ),
    //         cv::Scalar( 255, 0, 0 ),
    //         2,
    //         8,
    //         0
    //     );
    // }
    return std::make_tuple( last_classifier_detection_success, frame );
}
