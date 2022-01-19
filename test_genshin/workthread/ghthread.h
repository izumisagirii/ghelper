#ifndef GHTHREAD_H
#define GHTHREAD_H
#include <opencv2/opencv.hpp>
#include <QTreeWidgetItem>
#include <locate.h>
enum SCRIPT{
    USER_SCRIPT,TEST_LOCATE,TEST_TP
};
constexpr int SCRIPT_NUM = 2;
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
    std::thread _task;
    std::thread _ending;
    bool running = false;
    bool busy = false;
    maplocate GHmap;
    gwindow GHwindow;
};

#endif // GHTHREAD_H
