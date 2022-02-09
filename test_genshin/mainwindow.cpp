#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    this->setFocusPolicy(Qt::StrongFocus);
    hook.installHook(&hook);
    qRegisterMetaType<Hook::Type>("Type");
    //ui->listWidget->setViewMode(QListView::IconMode);
    //ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    on_reload_clicked();
    QObject::connect(ui->treeWidget,&QTreeWidget::itemChanged,this,&MainWindow::task_changed);
    connect(&hook,SIGNAL(sendKeyType(Type)),this,SLOT(checkType(Type)));
    log.setAttribute(Qt::WA_QuitOnClose,false);
    lab.setAlignment(Qt::AlignTop|Qt::AlignLeft);
    log.setStyleSheet("QLabel{font:bold 14px;color:rgb(255,0,0);}");
    lab.setMargin(10);
    lab.setStyleSheet("QLabel{font:12px;background-color:rgb(f9,f9,f9);}");
    lab.setWindowTitle("帮助");
    lab.setWordWrap(true);
    this->work_thread.setup(&this->log);
}

MainWindow::~MainWindow() {
    this->log.close();
    delete ui;
    hook.unInstallHook();
}

void MainWindow::reload_listview(){
    ui->listWidget->clear();
    for(auto &task:selected_task){
        ui->listWidget->addItem(QString::fromStdString(task->name));
    }
    //qDebug()<<"gengxinle";
}
void MainWindow::task_changed(){
    if(selected_task.empty()){
        for(auto &one_task:task_list){
            //qDebug()<<task_list.size();
            if(one_task.banner->checkState(0) == Qt::Checked){
                task *taskP = &one_task;
                selected_task.push_back(taskP);
                break;
            }
        }
    }else{
        for(auto &one_task:task_list){
            if(one_task.banner->checkState(0) == Qt::Unchecked){
                continue;
            }
            bool found = false;
            task *taskP = &one_task;
            for(auto &one_selected_task:selected_task){
                if (taskP == one_selected_task){
                    found = true;
                    break;
                }
            }
            if(!found){
                selected_task.push_back(taskP);
                //qDebug()<<"input";
                break;
            }
        }
        auto task_itr = selected_task.begin();
        //qDebug()<<selected_task.size();
        do{
            bool found = false;
            for(auto &one_task:task_list){
                if(one_task.banner->checkState(0) == Qt::Unchecked){
                    continue;
                }
                task *taskP = &one_task;
                if(taskP == *task_itr){
                    found = true;
                    break;
                }
            }
            //qDebug()<<found;
            if(!found){
                selected_task.erase(task_itr);
                break;
            }
            task_itr++;
        }while(task_itr!=selected_task.end());
    }
    this->reload_listview();
}
void MainWindow::on_reload_clicked()
{
    ui->listWidget->clear();
    scripts.setText(0,"内置脚本");
    user_scripts.setText(0,"自定义脚本");
    ui->treeWidget->addTopLevelItem(&scripts);
    ui->treeWidget->addTopLevelItem(&user_scripts);
    list.clear();
    user_list.clear();
    task_list.clear();
    selected_task.clear();
    //qDebug()<<list.capacity();
//    int task_num = 0;
    int user_task_num = 0;
    struct _finddata_t fileinfo;
    long fHandle;
    fHandle=_findfirst( (script_path+"*.ghs").c_str(), &fileinfo );
    if(!(fHandle == -1L)){
        do{
            QFile file((script_path+fileinfo.name).c_str());
            file.open(QIODevice::ReadOnly);
            QTextStream in(&file);
            QString str = in.readLine();
            str = str.simplified();
            if(str.startsWith("#NAME")){
                //qDebug()<<str;
                task_list.push_back(task{true,str.section(" ",1,1).toStdString(),script_path+fileinfo.name,SCRIPT::USER_SCRIPT,nullptr});
                user_task_num ++;
            }
            file.close();
        }while( _findnext(fHandle,&fileinfo)==0);
    }
    setList(task_list);

    user_list.reserve(user_task_num);
    list.reserve(SCRIPT_NUM);
    for(auto &task:task_list){
        QTreeWidgetItem temp;
        temp.setText(0,QString::fromStdString(task.name));
        temp.setCheckState(0,Qt::Unchecked);
        if(task.user){
            user_list.push_back(temp);
            task.banner = &user_list.back();
        }else{
            list.push_back(temp);
            task.banner = &list.back();
        }
    }
    for(auto &child:user_list){
        user_scripts.addChild(&child);
    }
    for(auto &child:list){
        scripts.addChild(&child);
    }
    _findclose( fHandle );
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //            std::thread show_help([this,_task]{ thread_show_help(_task.path); });
    //            show_help.detach();
    for(auto &_task:task_list){
        if(_task.banner == item){
            lab.setFixedSize(250,200);
            QString str;
//            if(_task.user){
            if(true){
                QFile file(_task.path.c_str());
                file.open(QIODevice::ReadOnly);
                QTextStream in(&file);

                while(!in.atEnd()){
                    str = in.readLine();
                    str = str.trimmed();
                    if(str.startsWith("#DISC")){
                        str = str.section ("#DISC",1);
                        while(!in.atEnd()){
                            QString str1 = in.readLine();
                            if(str1.startsWith("#")){
                                break;
                            }
                            str = str + "\n" +str1;
                        }
                        break;
                    }
                }
                if(str.isNull()||str.isEmpty()){
                    str = "此脚本说明为空";
                }

            }else{

            }
            lab.setText(str);
            lab.show();
            //lab.close();
        }
    }
}

//void MainWindow::thread_show_help(const std::string &path){
//    //    while (1) {
//    //        qDebug()<<path.c_str();
//    //        std::this_thread::sleep_for(std::chrono::seconds(1));
//    //    }

//}

void MainWindow::on_help_clicked()
{
    lab.setFixedSize(400,600);
    QFile file("../config/disc/help.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString str;
    while(!in.atEnd()){
        str = in.readLine();
        str = str.trimmed();
        if(str.startsWith("#DISC")){
            str = str.section ("#DISC",1);
            while(!in.atEnd()){
                QString str1 = in.readLine();
                if(str1.startsWith("#")){
                    break;
                }
                str = str + "\n" +str1;
                //qDebug()<<str;
            }
            break;
        }
    }
    if(str.isNull()||str.isEmpty()){
        str = "帮助文档为空";
    }
    //qDebug()<<str;
    lab.setText(str);
    lab.show();
}
//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    qDebug()<<event->text();
//    qDebug()<<event->key();//按键的枚举值
//}
void MainWindow::checkType(Hook::Type type){
    if(this->work_thread.start_task(this->selected_task)){
        this->on_reload_clicked();
    }
}

void MainWindow::on_run_clicked()
{
    checkType(Hook::F9);
}
