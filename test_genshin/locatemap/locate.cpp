#include "locate.h"
constexpr float kRatioThresh = 0.75;
constexpr double hessianThreshold = 20;
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
    for(auto kp:this->keypoint_map){
        this->keypoint_tree.BalanceInsert({kp.pt.x,kp.pt.y,&kp});
    }
    this->genshin_window->push_log("完毕！");
    //    featureDetector->compute(map_in, keyPoint, descriptor);
    //    std::vector<cv::KeyPoint> keyPoint_in;
    //    cv::Mat descriptor_in;
    //    cv::Mat input = cv::imread("test.png");
    //    cv::cvtColor(input, input, cv::COLOR_BGR2GRAY);
    //    featureDetector->detect(input, keyPoint_in);
    //    featureDetector->compute(input, keyPoint_in, descriptor_in);
    //    //    //创建基于FLANN的描述符匹配对象
    //    cv::FlannBasedMatcher matcher;
    //    //    std::vector<cv::Mat> desc_collection(1, descriptor);
    //    //    matcher.add(desc_collection);
    //    //    matcher.train();
    //    std::vector<std::vector<cv::DMatch>> matches;
    //    matcher.knnMatch(descriptor_in,descriptor, matches, 10);
    //    std::cout<<matches.size()<<std::endl;
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
    //    cv::imshow("test",match);
    //    cv::waitKey(1);
    cv::cvtColor(match, match, cv::COLOR_BGR2GRAY);
    cv::resize(match,match,cv::Size(match.cols/RateResize,match.rows/RateResize));
    std::vector<cv::KeyPoint> keyPoint_in;
    featureDetector->detect(match, keyPoint_in);
    //std::cout << keyPoint_in.size();
    featureDetector->compute(match, keyPoint_in, descriptor_in);
    cv::FlannBasedMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(descriptor_in,descriptor,matches);
    //matcher.knnMatch(descriptor_in,descriptor,matches,20);
    std::cout<<matches.size()<<std::endl;
    //    cv::FlannBasedMatcher matcher;
    //    std::vector<cv::Mat> desc_collection(1, descriptor);
    //    matcher.add(desc_collection);
    //    matcher.train();
    //    std::vector<std::vector<cv::DMatch>> matches;
    //    matcher.knnMatch(descriptor_in, matches, 10);
    //根据劳氏算法，得到优秀的匹配点
    std::vector<cv::DMatch> goodMatches;
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;
    //    for (auto &matche: matches) {
    //        if (matche[0].distance < (kRatioThresh+0.18) * matche[1].distance) {
    //            goodMatches.push_back(matche[0]);
    //            obj.push_back(keyPoint_in[matche[0].queryIdx].pt);
    //            scene.push_back(keypoint_map_resize[matche[0].trainIdx].pt);
    //        }
    //    }
    // 劳氏效果不好
    double min_dist = min_element( matches.begin(), matches.end(), [](const cv::DMatch& m1, const cv::DMatch& m2) {return m1.distance<m2.distance;} )->distance;
    //double max_dist = max_element( matches.begin(), matches.end(), [](const cv::DMatch& m1, const cv::DMatch& m2) {return m1.distance<m2.distance;} )->distance;
    //当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
    std::vector< cv::DMatch > good_matches;
    for ( int i = 0; i < descriptor_in.rows; i++ )
    {
        if ( matches[i].distance <= std::max ( 2*min_dist, 0.3 ) )
        {
            good_matches.push_back ( matches[i] );
            obj.push_back(keyPoint_in[matches[i].queryIdx].pt);
            scene.push_back(keypoint_map_resize[matches[i].trainIdx].pt);
        }
    }
    //std::cout<<"good"<<obj.size()<<std::endl;
    cv::Mat H;
    if (obj.size() > numThreshold) {
        H = estimateAffinePartial2D(obj, scene);
    }else{
        //std::cout<<obj.size()<<std::endl;
        //this->genshin_window->push_log("[info]size");
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
    QString text("[info]位置-x:");
    pos_mu.lock();
    text = text+QString::number(pos.location.x)+"y:"+QString::number(pos.location.y)+"T:"+QString::number(pos.towards);
    pos_mu.unlock();
    this->genshin_window->push_log(text);
}

bool maplocate::TPmap(int x, int y){
    genshin_window->key_click('M');
    Sleep(2000);
    genshin_window->click_wheel(100);
    Sleep(100);
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
