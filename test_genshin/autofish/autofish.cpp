#include "autofish.h"

fisher::fisher(gwindow* _window){
    this->window = _window;
    this->fishing = false;
    this->running = true;
    this->thread = std::thread([this]{this->task();});
    window->push_log("[start]准备就绪，去钓鱼吧!");
}

void fisher::quit(){
    this->running = false;
    this->thread.join();
}

void fisher::task(){
    while(running){
        if(fishing){
            cv::Mat fish_bar;
            this->window->get_frame_part(fish_bar,cv::Rect(477,66,325,19));
            cv::Mat fish_barA(fish_bar.rows,fish_bar.cols,CV_8UC1);
            cv::Mat out[] = {fish_barA};
            int fromTo[] = {3,0};
            cv::mixChannels(&fish_bar,1,out,1,fromTo,1);
            cv::threshold(fish_barA,fish_barA,120,255,cv::THRESH_BINARY);
            cv::Scalar sum = cv::sum(fish_barA);
            if(sum[0] > 10000){
                cv::Mat length = fish_barA(cv::Rect(0,10,325,1));
                sum = cv::sum(length);
                double pos = sum[0]/255;
                const uchar* indata = fish_barA.ptr<uchar>(4);
                int flag_num = 0;
                double flag_pos = 0;
                for(int i=0;i<fish_barA.cols*fish_barA.channels();i++){
                    if(indata[i]==255){
                        flag_num ++;
                        flag_pos = flag_pos + i;
                    }
                }
                flag_pos = flag_pos/flag_num;
                if(flag_pos>pos){
                    window->click_down();
                    window->click_up();
                    Sleep(20);
                }
            }else{
                //window->push_log("[info]暂停钓鱼");
                fishing = false;
            }
            Sleep(50);
        }else{
            cv::Mat fish_note;
            this->window->get_frame_part(fish_note,cv::Rect(589,132,93,20));
            cv::Mat fish_noteA(fish_note.rows,fish_note.cols,CV_8UC1);
            cv::Mat out[] = {fish_noteA};
            int fromTo[] = {3,0};
            cv::mixChannels(&fish_note,1,out,1,fromTo,1);
            cv::threshold(fish_noteA,fish_noteA,200,255,cv::THRESH_BINARY);
            cv::Scalar sum = cv::sum(fish_noteA);
            cv::Scalar suml = cv::sum(fish_noteA(cv::Rect(0,0,93,1)));
            if(sum[0] > 5000 && suml[0] < 1){
                fishing = true;
                window->click_down();
                window->click_up();
                window->push_log("[info]尝试钓鱼");
                Sleep(500);
            }
            Sleep(200);
        }
    }
}
