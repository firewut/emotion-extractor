#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <tuple>

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/core/core.hpp>

class Cascade {
    std::string title;
    std::string classifierFile;

    // Works as AND
    cv::CascadeClassifier cascadeClassifier;

    public:
        Cascade(
            std::string,
            std::string
        );
        std::tuple<std::vector<cv::Rect>, cv::Mat> detect(cv::Mat);
};

class Emotion {
    std::vector<Cascade> cascades;

    std::vector<int> appear_frames;
    std::vector<int> disappear_frames;

    void join_frames(int);

    public:
        std::string title;
        Emotion(
            std::string,
            std::vector<Cascade>
        );

        std::tuple<bool, cv::Mat> detect(cv::Mat);
        void event_at_frame(int, bool);
        void normalize_frames(int);

        std::vector< std::vector<std::string> > get_time_ranges(int);
};

#endif
