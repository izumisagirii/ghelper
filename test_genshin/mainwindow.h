#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include <QMainWindow>
#include <QtWidgets>
#include <QTreeWidgetItem>
#include "workthread/ghthread.h"
#include "hook.h"
#include <io.h>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QTextStream>
//#include <QMessageBox>
//#include<QKeyEvent>
#include <QThread>
#include <QSizePolicy>
#include "locate.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

//class TestTreeWidget : public QTreeWidget
//{
//    Q_OBJECT

//public:
//    TestTreeWidget (QWidget * parent = 0) : QTreeWidget(parent){}
//    ~TestTreeWidget (){ }

//signals:
//    void itemCheckStateChanged(QTreeWidgetItem*, bool);
//};

//class TestTreeWidgetItem : public QTreeWidgetItem
//{
//public:
//    TestTreeWidgetItem ();

//    TestTreeWidgetItem (int type = Type) :QTreeWidgetItem(type){ }
//    TestTreeWidgetItem (const QStringList & strings, int type = Type) : QTreeWidgetItem(strings, type){ }
//    TestTreeWidgetItem (QTreeWidget * parent, int type = Type) : QTreeWidgetItem(parent, type){}
//    TestTreeWidgetItem (QTreeWidget * parent, const QStringList & strings, int type = Type)
//        :QTreeWidgetItem(parent, strings, type){}
//    TestTreeWidgetItem (QTreeWidget * parent, QTreeWidgetItem * preceding, int type = Type)
//        : QTreeWidgetItem(parent, preceding, type){}
//    TestTreeWidgetItem (QTreeWidgetItem * parent, int type = Type) :QTreeWidgetItem(parent, type){}
//    TestTreeWidgetItem (QTreeWidgetItem * parent, const QStringList & strings, int type = Type)
//        :QTreeWidgetItem(parent, strings, type){}
//    TestTreeWidgetItem (QTreeWidgetItem * parent, QTreeWidgetItem * preceding, int type = Type)
//        :QTreeWidgetItem(parent, preceding, type){}

//    ~TestTreeWidgetItem (){}

//    void setData(int column, int role, const QVariant& value);
//};

//void TestTreeWidgetItem ::setData(int column, int role, const QVariant& value)
//{
//    const bool isCheckChange = column == 0
//        && role == Qt::CheckStateRole
//        && data(column, role).isValid() // Don't "change" during initialization
//        && checkState(0) != value;
//    QTreeWidgetItem::setData(column, role, value);
//    if (isCheckChange) {
//        TestTreeWidget *tree = static_cast<TestTreeWidget *>(treeWidget());
//        emit tree->itemCheckStateChanged(this, checkState(0) == Qt::Checked);
//    }
//}



class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    const std::string script_path = "../user_script/";
    QTreeWidgetItem scripts;
    QTreeWidgetItem user_scripts;
    std::vector<QTreeWidgetItem> list;
    std::vector<QTreeWidgetItem> user_list;
    std::vector<task> task_list;
    std::vector<task*> selected_task;
    QLabel lab;
    Hook hook;
    TransparentWidget log;
    //gwindow window_debug;
    //maplocate locate_test;
    GHthread work_thread;
    void reload_listview();
    //void keyPressEvent(QKeyEvent *event);
private slots:
    void on_reload_clicked();
    void task_changed();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_help_clicked();
    void checkType(Hook::Type);
    void on_run_clicked();
};
#endif // MAINWINDOW_H
