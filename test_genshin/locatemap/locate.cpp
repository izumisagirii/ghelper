#include "locate.h"
constexpr float kRatioThresh = 0.75;
constexpr double hessianThreshold = 40;
constexpr int numThreshold = 8;
constexpr int RateResize = 5;
maplocate::maplocate()
{
    featureDetector = cv::xfeatures2d::SURF::create(hessianThreshold, 4, 3);
}
void maplocate::setup(gwindow* genshin_window)
{
    this->genshin_window = genshin_window;
    //    cv::Mat test = cv::Mat::zeros(100, 100, CV_8UC3);;
    //    cv::imwrite("in.png",test);
}
void maplocate::loadmap(GHMAP map_id){
    this->clear_load();

    cv::FileStorage fs_read("../config/config.yml", cv::FileStorage::READ);
    std::string path;
    this->genshin_window->push_log("[1/3]正在加载地图...");
    switch (map_id) {
    case TEYVAT:
        fs_read["MapTeyvat"] >> path;
        break;
    case ENKANOMIYA:
        fs_read["MapEnkanomiya"] >> path;
        break;
    default:
        fs_read.release();
        this->genshin_window->push_log("[error]加载失败!");
        return;
    }
    fs_read.release();
    path.insert(0,"../config/");
    this->map = cv::imread(path);
    cv::cvtColor(map, map, cv::COLOR_BGR2GRAY);
    cv::resize(map,map_resize,cv::Size(map.cols/RateResize,map.rows/RateResize));
    //读取图片

    cv::Mat descriptor;
    //auto featureDetector = cv::xfeatures2d::SURF::create(hessianThreshold, 4, 3);
    //创建surf检测

    //判断是否进行检测
    path = path+".yml";
    if(access( path.c_str(), F_OK ) != -1){
        this->genshin_window->push_log("[2/3]正在从本地加载地图特征点...");
        cv::FileStorage fs(path, cv::FileStorage::READ);
        fs["KeyPoint"] >> this->keypoint_map;
        fs["keypoint_map_resize"] >> this->keypoint_map_resize;
        fs.release();
        this->genshin_window->push_log("加载完毕!");

    }else{
        this->genshin_window->push_log("[2/3]首次加载地图特征点，需要较长时间计算...");
        cv::FileStorage fs(path, cv::FileStorage::WRITE);
        featureDetector->detect(map, this->keypoint_map);
        featureDetector->detect(map_resize, this->keypoint_map_resize);
        this->genshin_window->push_log("计算完毕！正在存储特征点...");
        fs << "KeyPoint" << keypoint_map;
        fs << "keypoint_map_resize" << keypoint_map_resize;
        fs.release();
        this->genshin_window->push_log("特征点已存储!");
    }
    this->genshin_window->push_log("[3/3]正在使用四叉树处理特征点...");
    for(auto &kp:this->keypoint_map){
        this->keypoint_tree.BalanceInsert({kp.pt.x,kp.pt.y,&kp});
    }
    this->genshin_window->push_log("完毕！");
}

void maplocate::clear_load(){
    this->map.release();
    this->map_resize.release();
    this->keypoint_map.clear();
    this->keypoint_map_resize.clear();
    this->keypoint_tree.clear();
}

bool maplocate::match_map(){
    cv::Mat descriptor,descriptor_in;
    featureDetector->compute(this->map_resize, keypoint_map_resize, descriptor);
    cv::Mat match;
    this->genshin_window->get_frame_part(match,cv::Rect(390,110,500,500));
    cv::cvtColor(match, match, cv::COLOR_BGR2GRAY);
    cv::resize(match,match,cv::Size(match.cols/RateResize,match.rows/RateResize));
    std::vector<cv::KeyPoint> keyPoint_in;
    featureDetector->detect(match, keyPoint_in);
    featureDetector->compute(match, keyPoint_in, descriptor_in);
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptor_in,descriptor,matches);
    std::vector<cv::DMatch> goodMatches;
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;
    double min_dist = min_element( matches.begin(), matches.end(), [](const cv::DMatch& m1, const cv::DMatch& m2) {return m1.distance<m2.distance;} )->distance;
    std::vector< cv::DMatch > good_matches;
    for ( int i = 0; i < descriptor_in.rows; i++ )
    {
        if ( matches[i].distance <= std::max ( 2.0*min_dist, 0.3 ) )
        {
            good_matches.push_back ( matches[i] );
            obj.push_back(keyPoint_in[matches[i].queryIdx].pt);
            scene.push_back(keypoint_map_resize[matches[i].trainIdx].pt);
        }
    }
    cv::Mat H;
    if (obj.size() > numThreshold) {
        H = estimateAffinePartial2D(obj, scene);
    }else{
        return false;
    }
    if (H.empty()){
        this->genshin_window->push_log("[info]H");
        return false;
    }
    std::vector<cv::Point2f>obj_test(1,cv::Point2f(250/RateResize,250/RateResize));
    cv::transform(obj_test, obj_test, H);
    pos_mu.lock();
    this->pos.location = obj_test[0] * RateResize;
    pos_mu.unlock();
    //print_pos();                       //test for debug
}
void maplocate::print_pos(){
    QString text("[info]位置 x:");
    pos_mu.lock();
    text = text+QString::number(pos.location.x)+" y:"+QString::number(pos.location.y)+" 朝向 T:"+QString::number(pos.towards);
    pos_mu.unlock();
    this->genshin_window->push_log(text);
}

bool maplocate::TPmap(int x, int y){
    genshin_window->key_click('M');
    Sleep(4000);
    genshin_window->click_wheel(100);
    Sleep(200);
    genshin_window->click_wheel(-25);
    Sleep(300);
    uchar SAFE_QUIT_TIME = 5;
    while(SAFE_QUIT_TIME>0){
        if(this->match_map()){
            break;
        }
        SAFE_QUIT_TIME--;
    }
    if(SAFE_QUIT_TIME == 0){
        return false;
    }
    cv::Point2f dis = this->pos.location - cv::Point2f(x,y);
    bool level = dis.x>0?true:false;
    bool vertical = dis.y>0?true:false;
    int X = abs((int)dis.x)/130;
    int Y = abs((int)dis.y)/130;
    while(X>0||Y>0){
        genshin_window->drag_map(level,vertical,X>0?true:false,Y>0?true:false);
        Sleep(500);
        X = X-2;
        Y = Y-2;
    }
    this->match_map();
    genshin_window->click(x-pos.location.x+640,y-pos.location.y+360);
    Sleep(800);
    if(!genshin_window->color_judge(1050,675,{102,83,74})){
        genshin_window->click(890,525);
        Sleep(800);
    }
    genshin_window->click(1050,675);
}
void maplocate::force_quit(){
    if(tracking){
        this->stop_track();
    }
    this->clear_load();
    this->pos = {{0,0},0,false};
}
void maplocate::start_track(int fps){
    this->_track_relocate_map();
    this->tracking = true;
    this->track_thread = std::thread([this,fps]{ _track_thread(fps); });
}
void maplocate::stop_track(){
    this->tracking = false;
    this->track_thread.join();
    this->genshin_window->push_log("追踪坐标停止");
    this->pos.tracking = false;
}
void maplocate::_track_thread(int fps){
    this->genshin_window->push_log("开始追踪坐标");
    DWORD frame_time =(fps>10||fps<=0)? 100:1000/fps;
    while(this->tracking){
        DWORD start_t = GetTickCount();
        this->pos.tracking?this->_track_locate_map():this->_track_relocate_map();
        DWORD end_t = GetTickCount();
        if(end_t-start_t<frame_time){
            Sleep(start_t+frame_time-end_t);
        }
    }
}
void maplocate::_track_relocate_map(){
    genshin_window->snatch_map();
    uchar SAFE_QUIT_TIME = 5;
    while(SAFE_QUIT_TIME>0){
        if(this->match_map()){
            this->pos.tracking = true;
            break;
        }
        SAFE_QUIT_TIME--;
    }
    genshin_window->release_map();
    this->print_pos();
}
void maplocate::_track_locate_map(){
    cv::Mat descriptor,descriptor_in;
    std::vector<cv::KeyPoint> keyPoint_selected;
    std::vector<void*> keyPointP;
    int visitNum = 0;
    int foundNum = 0;
    cv::Point2f nowpos = this->pos.location;
    cv::Rect selectrect(nowpos.x-100,nowpos.y-100,200,200);
    if (selectrect!=(selectrect&cv::Rect(0,0,map.cols,map.rows))){
        pos.tracking = false;
        return;
    }
    this->keypoint_tree.RegionResearch(keyPointP,nowpos.x-80,nowpos.x+80,nowpos.y-80,nowpos.y+80,visitNum,foundNum);
    if(foundNum<20){
        pos.tracking = false;
        return;
    }
    keyPoint_selected.reserve(keyPointP.size());
    for(const auto &kpp:keyPointP){
        keyPoint_selected.push_back(*(cv::KeyPoint*)kpp);
        keyPoint_selected.back().pt = keyPoint_selected.back().pt - nowpos + cv::Point2f(100,100);
    }
//    cv::FileStorage fs("1.yml", cv::FileStorage::WRITE);
//    fs << "keypoint" << keyPoint_selected;
//    fs.release();
    //DWORD start_t = GetTickCount();
//    cv::Mat selected_map;
//    this->map(selectrect).copyTo(selected_map);
    cv::Mat selected_map = this->map(selectrect);
    featureDetector->compute(selected_map, keyPoint_selected, descriptor);
    //DWORD end_t = GetTickCount();
    //this->genshin_window->push_log(QString::number(end_t-start_t)+"compute");
    cv::Mat match;
    this->genshin_window->get_frame_part(match,cv::Rect(60,31,100,100));
    cv::cvtColor(match, match, cv::COLOR_BGR2GRAY);
    //cv::resize(match,match,cv::Size(match.cols/RateResize,match.rows/RateResize));
    std::vector<cv::KeyPoint> keyPoint_in;
    featureDetector->detect(match, keyPoint_in);
    featureDetector->compute(match, keyPoint_in, descriptor_in);
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptor_in,descriptor,matches);
    //this->genshin_window->push_log(QString::number(matches.size())+"match");
    std::vector<cv::DMatch> goodMatches;
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;
    double min_dist = min_element( matches.begin(), matches.end(), [](const cv::DMatch& m1, const cv::DMatch& m2) {return m1.distance<m2.distance;} )->distance;
    std::vector< cv::DMatch > good_matches;
    for ( int i = 0; i < descriptor_in.rows; i++ )
    {
        if ( matches[i].distance <= std::max ( 2*min_dist, 0.43 ) )
        {
            good_matches.push_back ( matches[i] );
            obj.push_back(keyPoint_in[matches[i].queryIdx].pt);
            scene.push_back(keyPoint_selected[matches[i].trainIdx].pt);
        }
    }
    cv::Mat H;
    //this->genshin_window->push_log(QString::number(obj.size())+"goodmatch");
    if (obj.size() > numThreshold) {
        H = estimateAffinePartial2D(obj, scene);
    }else{
        pos.tracking = false;
        return;
    }
    if (H.empty()){
        this->genshin_window->push_log("[info]H");
        pos.tracking = false;
        return;
    }
    std::vector<cv::Point2f>obj_test(1,cv::Point2f(50,50));
    cv::transform(obj_test, obj_test, H);
    double theta = atan2(H.at<double>(0,0),H.at<double>(1,0));
    theta = theta*180/M_PI + 180;
    pos_mu.lock();
    this->pos.location = obj_test[0] + nowpos - cv::Point2f(100,100);
    this->pos.towards = theta;
    pos_mu.unlock();
    this->print_pos();
}
