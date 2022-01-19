#include "hook.h"
#include <QDebug>

static HHOOK keyHook=nullptr;//钩子对象
static Hook* hook;//Qt界面中调用Hook类的对象
LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam){//钩子消息函数，系统消息队列信息会返回到该函数中
    KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;//lParam用于判断按键类型
    if(wParam == WM_KEYDOWN){//wParam用于判断事件类型，当前为按键按下事件
        if(pkbhs->vkCode == VK_F9&&GetAsyncKeyState(VK_CONTROL)){//按下
            hook->sendSignal(Hook::F9);//安装钩子的对象发出按键监听信号
        }
        if(pkbhs->vkCode == VK_F10&&GetAsyncKeyState(VK_CONTROL)){//按
            hook->sendSignal(Hook::F10);
        }
    }
    return CallNextHookEx(keyHook,nCode,wParam,lParam);//继续原有的事件队列
}

Hook::Hook()
{

}

void Hook::installHook(Hook* h){
    keyHook = SetWindowsHookEx(WH_KEYBOARD_LL,keyProc,nullptr,0);
    hook = h;
}
void Hook::unInstallHook(){
    UnhookWindowsHookEx(keyHook);
    keyHook = nullptr;
}

void Hook::sendSignal(Type type){
    emit sendKeyType(type);
}
