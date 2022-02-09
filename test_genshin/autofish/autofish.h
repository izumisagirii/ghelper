#ifndef AUTOFISH_H
#define AUTOFISH_H
#include <gwindow.h>
class fisher
{
public:
    fisher(gwindow* _window);
    void quit();
private:
    void task();
    std::thread thread;
    gwindow* window;
    bool fishing;
    bool running;
};

#endif
