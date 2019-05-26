#ifndef DEFINE_H
#define DEFINE_H
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
#include <QVector>
#include<iostream>

//进程控制块，包括11项信息，记录了操作系统所需的、用于描述进程的当前状态和控制进程的全部信息
struct pcb{
   int pID;     //进程标识信息
   int pState;  //0-空闲，1-ready，3-wait，2-run 终止则执行terminalPro(int PID,int list)
   int prior;   //0最大，优先级，CPU调度时需要

   int pageNum;  //页数
   QVector<QVector<int> > page;  //页表
   QVector<QVector<int> > program; //指令类型+时间  CPU/IO

   int PC;       //程序计数器，表示下一条要执行的指令
   int instNum;  //总指令数

   /*在run或者ready 此为cpu指令的执行时间，若在wait队列，为IO等待时间*/
   int curInstNeedTime;  //当前指令总需要运行时间
   int curInstRunTime;   //当前指令已经运行时间

   QDateTime comingTime;//进程creat的时间
};
typedef struct pcb PCB;

// 进程控制块-链表形式
struct pcbList{
   PCB process;
   struct pcbList* next;
};
typedef struct pcbList PCBList;

struct pagetable{
   int PID;
   QDateTime comingTime; //调入时间
   QDateTime LRUTime;  //最近使用时间
};
typedef struct pagetable pageTable;

#endif // DEFINE_H
