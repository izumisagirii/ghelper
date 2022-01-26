#include "ghthread.h"
#include<windows.h>
void setList(std::vector<task> &task_list){
    task_list.push_back(task{false,"测试_定位","",SCRIPT::TEST_LOCATE,nullptr});
    task_list.push_back(task{false,"测试_传送","",SCRIPT::TEST_TP,nullptr});
    task_list.push_back(task{false,"测试_实时定位","",SCRIPT::TEST_TRACK,nullptr});
}


GHthread::GHthread()
{

}
void GHthread::setup(TransparentWidget *log){
    this->GHwindow.setup(log);
    this->GHmap.setup(&(this->GHwindow));
}
void GHthread::test()
{
    cv::Mat test = cv::Mat::zeros(100, 100, CV_8UC3);;
    cv::imwrite("in.png",test);
}
bool GHthread::start_task(std::vector<task *> task_list){
    if (running){
        running = false;
        busy = true;
        this->GHwindow.push_log("正在结束任务...");
        this->_ending = std::thread([this]{ wait_task_end(); });
        return false;
    }else{
        if(busy){
            this->GHwindow.push_log("结束当前步骤后程序将停止,请等待...");
            return false;
        }
        running = true;
        this->GHwindow.push_log("开始运行任务");
        if(this->_ending.joinable()){
            _ending.join();
        }
        if(this->_task.joinable()){
            _task.join();
        }
        this->_task = std::thread([this,task_list]{ thread_task(task_list); });
        return true;
    }
}
void GHthread::wait_task_end(){
    this ->_task.join();
    this->GHwindow.push_log("任务被强制结束!");
    busy = false;
}
void GHthread::thread_task(const std::vector<task *> &task_list){
    for(auto &_task:task_list){
        bool reg = true;
        switch (_task->index) {
        case SCRIPT::TEST_LOCATE:
            reg=TASK_testLOCATE();
            break;
        case SCRIPT::TEST_TP:
            reg=TASK_testTP();
            break;
        case SCRIPT::TEST_TRACK:
            reg=TASK_testTRACK();
            break;
        case SCRIPT::USER_SCRIPT:
            break;
        }

        if(!reg){
            running = false;
            return;
        }
    }
    running = false;
    this->GHwindow.push_log("[finish]所有任务完成!");
}

bool GHthread::TASK_testLOCATE(){
    this->GHwindow.set_window();
    this->GHmap.loadmap(TEYVAT);
    this->GHwindow.start_grab(10);
    while(running){
        this->GHmap.match_map();
        this->GHmap.print_pos();
    }
    this->GHmap.clear_load();
    this->GHwindow.stop_grab();
    this->GHwindow.push_log("[finish]测试定位完成");
    return true;
}

bool GHthread::TASK_testTP(){
    this->GHwindow.set_window();
    this->GHmap.loadmap(TEYVAT);
    this->GHwindow.start_grab(10);
    this->GHmap.TPmap(8310,7939);
    this->GHmap.clear_load();
    this->GHwindow.stop_grab();
    this->GHwindow.push_log("[finish]测试传送完成");
    return true;
}

bool GHthread::TASK_testTRACK(){
    this->GHwindow.set_window();
    this->GHmap.loadmap(TEYVAT);
    this->GHwindow.start_grab(10);
    this->GHmap.start_track();
    while(true){
        QUIT_CHECK
        Sleep(1000);
    }
}
