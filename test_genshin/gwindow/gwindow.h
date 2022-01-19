#ifndef GWINDOW_H
#define GWINDOW_H
#include <opencv2/opencv.hpp>
#include<windows.h>
#include <QtWidgets>
#include <thread>

class TransparentWidget : public QLabel
{
    Q_OBJECT
public:
    TransparentWidget(QLabel *parent = 0)
        : QLabel(parent)
    {
        this->setWindowTitle(QString::fromLocal8Bit("working..."));
        //this->setFixedSize(480, 320);
        this->setGeometry(300,10,600,320);
        this->setAlignment(Qt::AlignTop|Qt::AlignLeft);
        this->setWordWrap(true);
        this->setWindowFlags(Qt::WindowStaysOnTopHint);
        this->setWindowFlag(Qt::FramelessWindowHint);
        this->setAttribute(Qt::WA_TranslucentBackground);
        //this->setText("hello world!");
    }

//    void paintEvent(QPaintEvent *)
//    {
//        QPainter painter(this);
//        painter.fillRect(this->rect(), QColor(0, 0, 0, 50)); /* 设置透明颜色 */
//    }

    void pushText(const QString & input){
        text.insert(text.begin(),input);
        text.pop_back();
        QString _text;
        for(const auto &t:text){
            _text = _text + t +"\n";
        }
        this->setText(_text);
    }
    void clearText(){
        text = std::vector<QString> (10);
    }
    void pushAndShowThreadSafe(const QString & input)
    {
        show_mu.lock();
        this->pushText(input);
        this->show();
        show_mu.unlock();
    }
private:
    std::mutex show_mu;
    std::vector<QString> text = std::vector<QString> (10);
};


class gwindow
{
public:
    gwindow(int x = 1280,int y = 720);
    ~gwindow();
    void setup(TransparentWidget* log);
    bool set_window();
    bool start_grab(int fps = 10);
    void stop_grab();
    void get_frame(cv::Mat &pic);
    void get_frame_part(cv::Mat &pic,const cv::Rect &rect);
    bool get_one_frame(cv::Mat &pic);
    void click(int x,int y);
    void click_drag(int x,int y,int _x,int _y);
    void click_down();
    void click_up();
    void click_mid();
    void click_wheel(int num);
    void relative_move(int x,int y);
    void absolute_move(int x,int y);
    void key_click(BYTE bVk);
    void key_down(BYTE bVk);
    void key_up(BYTE bVk);
    void push_log(const QString & input);
    bool color_judge(int x,int y,cv::Vec3b rgb);
    bool drag_map(bool level,bool vertical,bool len,bool ven);
private:
    int _x,_y;
    bool _grabing;
    TransparentWidget* _log = nullptr;
    cv::Mat _frame;
    HWND _window = nullptr;
    std::mutex _frame_mu;
    std::recursive_mutex  _window_mu;
    std::thread _grab;
    void grab_thread(int fps);
    void HBitmapToMat(HBITMAP& _hBmp,cv::Mat& _mat);
};

#endif // GHTHREAD_H
