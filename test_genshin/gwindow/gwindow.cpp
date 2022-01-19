#include "gwindow.h"

gwindow::gwindow(int x,int y)
{
    this -> _x = x;
    this -> _y = y;
}
gwindow::~gwindow()
{

}
void gwindow::setup(TransparentWidget* log)
{
    _log = log;
}
bool gwindow::set_window()
{
    char genshin_name[]={-44,-83,-55,-15,0};//避免GBK编码错误
    this->_window = FindWindow("UnityWndClass",genshin_name);
    if(_window == nullptr)
    {
        _window = FindWindow("UnityWndClass","GenshinImpact");
        if(_window == nullptr)
        {
            this->_log->pushAndShowThreadSafe("error:原神未在运行!");
            return false;
        }
    }
    this->_log->pushAndShowThreadSafe("检测到原神窗口");
    RECT pos;
    ShowWindow(_window, SW_SHOWNORMAL);
    SetForegroundWindow(_window);
    GetClientRect(_window,&pos);
    if(!(pos.right == _x&&pos.bottom == _y))
    {
        this->_log->pushAndShowThreadSafe("未按照指定分辨率运行");
        this->_window = nullptr;
        return false;
    }
    GetWindowRect(_window,&pos);
    SetWindowPos(_window, NULL, 0,0, pos.right-pos.left,pos.bottom-pos.top, SWP_SHOWWINDOW);
}
bool gwindow::get_one_frame(cv::Mat &pic){
    if(this->_window == nullptr){
        return false;
    }
    RECT client_rect;
    GetClientRect(_window, &client_rect);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    HDC hdcScreen = GetDC(_window);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdc, hbmp);

    BitBlt(hdc, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    this->HBitmapToMat(hbmp,pic);

    DeleteDC(hdc);
    DeleteObject(hbmp);
    ReleaseDC(nullptr, hdcScreen);
    return true;
}
bool gwindow::start_grab(int fps){
    this->_grabing = true;
    if(!this->get_one_frame(this->_frame)){return false;}
    this->_grab = std::thread([this,fps]{ grab_thread(fps); });
    this->_log->pushAndShowThreadSafe("开始采集窗口");
    return true;
}
void gwindow::stop_grab(){
    this->_grabing = false;
    this->_grab.join();
    this->_frame.release();
}
void gwindow::grab_thread(int fps){
    RECT client_rect;
    GetClientRect(_window, &client_rect);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;
    HDC hdcScreen = GetDC(_window);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdc, hbmp);
    int frame_time =(fps>100||fps==0)? 0:1000/fps;
    while(_grabing){
        DWORD start_t = GetTickCount();
        BitBlt(hdc, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);
        this->_frame_mu.lock();
        this->HBitmapToMat(hbmp,this->_frame);
        this->_frame_mu.unlock();
        DWORD end_t = GetTickCount();
        if(end_t-start_t<frame_time){
            Sleep(start_t+frame_time-end_t);
        }
//            cv::imshow("test_gs",this->_frame);
//            cv::waitKey(1);
    }
}
void gwindow::HBitmapToMat(HBITMAP &_hBmp, cv::Mat &_mat){
    //BITMAP操作
    BITMAP bmp;
    GetObject(_hBmp,sizeof(BITMAP),&bmp);
    int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8 ;
    //mat操作
    cv::Mat v_mat;
    v_mat.create(cv::Size(bmp.bmWidth,bmp.bmHeight), CV_MAKETYPE(CV_8U,nChannels));
    GetBitmapBits(_hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,v_mat.data);
    _mat=v_mat;
}
void gwindow::get_frame(cv::Mat &pic){
    this->_frame_mu.lock();
    _frame.copyTo(pic);
    this->_frame_mu.unlock();
}
void gwindow::get_frame_part(cv::Mat &pic,const cv::Rect &rect){
    this->_frame_mu.lock();
    _frame(rect & cv::Rect(0,0,_frame.cols,_frame.rows)).copyTo(pic);
    this->_frame_mu.unlock();
}
void gwindow::click(int x, int y){
    this->_window_mu.lock();
    POINT pt{x,y};
    ClientToScreen(_window,&pt);
    SetCursorPos(pt.x,pt.y);
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    this->_window_mu.unlock();
}
void gwindow::relative_move(int x, int y){
    this->_window_mu.lock();
    //    POINT pt{x,y};
    //    GetCursorPos(&pt);
    //ClientToScreen(_window,&pt);
    //    SetCursorPos(pt.x+x,pt.y+y);
    mouse_event(MOUSEEVENTF_MOVE,x,y,0,0);
    this->_window_mu.unlock();
}
void gwindow::key_click(BYTE bVk){
    BYTE scanCode = MapVirtualKey(VK_SPACE, 0);
    this->_window_mu.lock();
    keybd_event(bVk,scanCode,0,0);
    keybd_event(bVk,scanCode,KEYEVENTF_KEYUP,0);
    this->_window_mu.unlock();
}
void gwindow::key_down(BYTE bVk){
    BYTE scanCode = MapVirtualKey(VK_SPACE, 0);
    this->_window_mu.lock();
    keybd_event(bVk,scanCode,0,0);
    //keybd_event(bVk,scanCode,KEYEVENTF_KEYUP,0);
    this->_window_mu.unlock();
}
void gwindow::key_up(BYTE bVk){
    BYTE scanCode = MapVirtualKey(VK_SPACE, 0);
    this->_window_mu.lock();
    //keybd_event(bVk,scanCode,0,0);
    keybd_event(bVk,scanCode,KEYEVENTF_KEYUP,0);
    this->_window_mu.unlock();
}
void gwindow::click_mid(){
    this->_window_mu.lock();
    mouse_event(MOUSEEVENTF_MIDDLEDOWN,0,0,0,0);
    mouse_event(MOUSEEVENTF_MIDDLEUP,0,0,0,0);
    this->_window_mu.unlock();
}
void gwindow::click_wheel(int num){
    int i = abs(num);
    int h = num/i;
    this->_window_mu.lock();
    while(i>0){
        mouse_event(MOUSEEVENTF_WHEEL,0,0,h*WHEEL_DELTA,0);
        i--;
    }
    this->_window_mu.unlock();
}
void gwindow::click_down(){
    this->_window_mu.lock();
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    this->_window_mu.unlock();
}
void gwindow::click_up(){
    this->_window_mu.lock();
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    this->_window_mu.unlock();
}
void gwindow::click_drag(int x, int y, int _x, int _y){
    this->_window_mu.lock();
    POINT pt{x,y};
    ClientToScreen(_window,&pt);
    SetCursorPos(pt.x+_x,pt.y+_y);
    //Sleep(200);
    mouse_event(MOUSEEVENTF_LEFTDOWN,0,0,0,0);
    Sleep(300);
    SetCursorPos(pt.x,pt.y);
    Sleep(300);
    SetCursorPos(pt.x+_x,pt.y+_y);
    Sleep(300);
    mouse_event(MOUSEEVENTF_LEFTUP,0,0,0,0);
    //Sleep(500);
    this->_window_mu.unlock();
}
void gwindow::absolute_move(int x, int y){
    this->_window_mu.lock();
    POINT pt{x,y};
    ClientToScreen(_window,&pt);
    SetCursorPos(pt.x,pt.y);
    this->_window_mu.unlock();
}
void gwindow::push_log(const QString &input){
    this->_log->pushAndShowThreadSafe(input);
}
bool gwindow::color_judge(int x, int y,cv::Vec3b rgb){
    QString text("[info]color:");
    this->_frame_mu.lock();
    auto color = _frame.at<cv::Vec4b>(cv::Point(x,y));
//    text = text + QString::number(_frame.channels()) + " ";
    this->_frame_mu.unlock();

//    text = text + QString::number(color[0])+" "+QString::number(color[1])+" "+QString::number(color[2])+" "+QString::number(color[3]);
//    this->push_log(text);
    if(rgb[0] == color[0]&&rgb[1]==color[1]&&rgb[2]== color[2]){
        return true;
    }
    return false;
}

bool gwindow::drag_map(bool level, bool vertical,bool len,bool ven){
    this->_window_mu.lock();
    int x = level?490:790;
    int y = vertical?210:510;
    int _x = level?300:-300;
    int _y = vertical?300:-300;
    uchar SAFE_QUIT_TIME = 5;
    while(SAFE_QUIT_TIME>0){
        this->click_drag(x,y,_x*len,_y*ven);
        Sleep(700);
        if(this->color_judge(1237,32,cv::Vec3b(216,229,236))){
            this->_window_mu.unlock();
            return true;
        }else{
            this->key_click(VK_ESCAPE);
            Sleep(700);
            x = x + 30;
        }
        SAFE_QUIT_TIME --;
    }
    this->_window_mu.unlock();
    return false;
}
