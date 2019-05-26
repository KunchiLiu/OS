#include "define.h"

//struct pcb{
//   int pID;
//   int pState;  //0-空闲，1-ready，3-wait，2-run
//   int prior;  //0最大

//   int pageNum;
//   int** page;  //页表
//   int** program;

//   int PC;
//   int instNum;  //总指令数

//   /*在run或者ready 此为cpu指令的执行时间，若在wait队列，为IO等待时间*/
//   int curInstNeedTime;  //当前指令总需要运行时间
//   int curInstRunTime;   //当前指令已经运行时间

//   QDateTime comingTime;//进程到达的时间
//};
//typedef struct pcb PCB;

//struct pcbList{
//   PCB process;
//   struct pcbList* next;
//};
//typedef struct pcbList PCBList;

//struct pagetable{
//   int PID;
//   QDateTime comingTime; //调入时间
//   QDateTime LRUTime;  //最近使用时间
//};
//typedef struct pagetable pageTable;
