#ifndef HOOK_H
#define HOOK_H
#include <windows.h>
#include <QObject>
class Hook:public QObject
{
    Q_OBJECT
public:
    Hook();
    enum Type{F9,F10};//自定义枚举，定义“修改”、“撤回”两种状态
    void installHook(Hook*);//安装钩子函数，指针参数为了发送信号用
    void unInstallHook();//删除钩子
    void sendSignal(Type type);
    Q_ENUM(Type)//注册枚举类型进元对象系统，否则无法将枚举类型作为信号参数传递
signals:
    void sendKeyType(Type);//发送按键监控信号
};

#endif // HOOK_H
