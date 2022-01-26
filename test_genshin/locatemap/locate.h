#ifndef LOCATE_H
#define LOCATE_H
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <unistd.h>
#include "gwindow.h"
#include "qtree.h"
//#include <math.h>
enum GHMAP{
    TEYVAT,ENKANOMIYA,
};

struct GHpos{
    cv::Point2f location;
    float towards;
    bool tracking;
};

class maplocate
{
public:
    maplocate();
    void setup(gwindow* genshin_window);
    void loadmap(GHMAP map_id);
    bool match_map();
    void clear_load();
    bool TPmap(int x,int y);
    void print_pos();
    void start_track(int fps = 5);
    void stop_track();
    void force_quit();
private:
    gwindow* genshin_window = nullptr;
    cv::Ptr<cv::xfeatures2d::SURF> featureDetector;
    std::vector<cv::KeyPoint> keypoint_map;
    std::vector<cv::KeyPoint> keypoint_map_resize;
    cv::Mat map;
    cv::Mat map_resize;
    bool tracking = false;
    struct GHpos pos = {{0,0},0,false};
    std::mutex pos_mu;
    QuardTree<float> keypoint_tree;
    std::thread track_thread;
    void _track_thread(int fps);
    void _track_map();
    void _track_relocate_map();
    void _track_locate_map();
};

#endif
