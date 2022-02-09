#ifndef GHTHREAD_H
#define GHTHREAD_H
#include <opencv2/opencv.hpp>
#include <QTreeWidgetItem>
#include <locate.h>
#include <autofish.h>

#define QUIT_CHECK if(!running){ \
    GHmap.force_quit();  \
    GHwindow.force_quit();  \
    return false;  \
}

enum SCRIPT{
    USER_SCRIPT,TEST_LOCATE,TEST_TP,TEST_TRACK,TEST_CAP,AUTO_FISH
};
constexpr int SCRIPT_NUM = 5;
struct task{
    bool user;
    std::string name;
    std::string path;
    SCRIPT index;
    QTreeWidgetItem* banner;
};

void setList(std::vector<task> &task_list);


class GHthread
{
public:
    GHthread();
    void test();
    bool start_task(std::vector<task*>task_list);
    void setup(TransparentWidget* log);
private:
    void thread_task(const std::vector<task*>&task_list);
    void wait_task_end();
    bool TASK_testTP();
    bool TASK_testLOCATE();
    bool TASK_testTRACK();
    bool TASK_testCAP();
    bool TASK_AUTOFISH();
    std::thread _task;
    std::thread _ending;
    bool running = false;
    bool busy = false;
    maplocate GHmap;
    gwindow GHwindow;
};

#endif // GHTHREAD_H
