#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"define.h"
#include <QMainWindow>
#include <time.h>
#include <QString>
#include <windows.h>
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QtGui>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>
#include <QTextCodec>
#include <QSplitter>
#include <QListWidget>
#include <QTimer>
#include <QProgressBar>
#include <QTimer>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMainWindow>
#include<QMenu>
#include<QMap>
#include"dialog.h"
#include"file_dialog.h"
#include"create_dialog.h"
#include"define.h"
//初始化窗口

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void  sendData(PCBList*,PCBList*,PCBList*,int,int,pageTable*);   //emit sendData(ui->lineEdit->text());
    void  sendData_PID(QMap<int, int>);
public:
    Dialog *pro;
    File_Dialog *Fpro;
    Create_Dialog *Cpro;

private:
    Ui::MainWindow *ui;


   //队列  头指针不存放数据
   PCBList* waitingList;
   PCBList* readyList;
   PCBList* runningList;
   PCBList* breakList;

   int waitNum,readyNum; //队列中PCB的个数

   int totalPCBNum = 0;
   int curProStrategy = 2; //2-轮询 1-优先级 0-FCFS

   int curPageStrategy = 1;//0-FIFO,1-LRU
   int totalPageNum = 10;  //总页数
   int CPUState; //0-free,1-run

   int nowBreak = 0;  //1-有中断


   pageTable mainPageTable[10]; //总页表，max：50
   QTimer * CPUtimer; //时间片

   QMap<int, int> PIDmap;


   enum ProcessState{ New = 0, ready, run, wait, terminal };//进程状态
   enum Dispatch{fifs = 0,noPreePrio,timeSchedule};


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();




    //获取三个队列
    PCBList* getReadyList();
    PCBList* getRunningList();
    PCBList* getWaitingList();



    /*页表*/
    //确认页是否存在于内存,若存在，则返回1，且更新LRU。若不存在，则返回0
    int isExitPage(int PID,int pageNum);
    //申请新页
    int requestNewPage(int pageNum, int PID, int* pageTable);  //pageNum>1，返回是否成功，1-ok
    int requestNewPage_one(int PID);  //pageNum=1,返回物理页号
    //获取某页的调入时间
    QDateTime getPageComingTime(int num);
    //获取某页的LRUTIME
    QDateTime getPageLRUTime(int num);

    //换页算法
    int FIFOPage();
    int LRUpage();

    /*队列与调度   （头指针不存放数据）*/
    //时间片调度算法，每次从就绪队列中将第一个进程调入执行队列，再将执行队列中的PCB放入Ready的队列末尾
    void TimeSchedule();

    //优先级调度算法，每次从就绪队列中将优先级最高的进程调入执行队列
    void PrioSchedule();

    //先来先服务调度算法，每次从就绪队列中将最先到达的进程调入执行队列
    void FIFS();

    //将一个进程放入waiting队列,返回是否执行成功，1-ok
    int addWaitingList(PCBList* newPCB);

    //遍历waiting队列，使等待时间加1，判断是否可以调入ready队列，返回值为：1-有进程加入了ready队列，0-无
    int searchWaitingList();

    //将一个进程加入到ready队列的末尾
    int addReadyList(PCBList* newPCB);

    //总接口
    /*1.先判断run队列是否有进程。2.若无，根据调度策略，从ready中选择进程，注意判断ready中是否有进程。
     * 3.若有，根据调度策略进行不同的操作。4.若为轮询，则正常选择。5.若为FCFS，根据searchWaitingList（）的返回值，
     * 若为1，从ready中选择一个时间最早的进程与当前进程进行判断，在决定是否调换。6.若为优先级，类似
     * 返回值：run队列中是否发生的变化，1-有*/
    int processSchedule(int change);

    //判断run队列是否为空 1-空
    int isRunListEmpty();


    int runProcess();
    void writeFile();
    int runBreakProcess();

private slots:
    void on_process_clicked();
    void on_file_clicked();
    void on_cre_clicked();
    void on_br_clicked();
    void handleCPUSlice();

    //创建新进程
    void createPro(int PID, int prior, int fileIndex);
    void changeStrategy(int pro,int page);
    void terminalPro(int PID, int list);

};

#endif // MAINWINDOW_H
