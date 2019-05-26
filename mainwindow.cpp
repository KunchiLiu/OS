#include "mainwindow.h"
#include "ui_mainwindow.h"

#include"define.h"
#include <QTextCodec>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QInputDialog>
#include <QAction>
#include <QTextCodec>
#include <QTimer>
#include <QPalette>
#include<QMessageBox>
#include<QFile>
#include<QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //队列初始化
    waitingList = new PCBList();
    waitingList->next = NULL;

    readyList = new PCBList();
    readyList->next = NULL;

    runningList = new PCBList();
    runningList->next = NULL;

    breakList = new PCBList();
    breakList->next = NULL;


    waitNum= 0;
    readyNum = 0;

//    mainPageTable = (pageTable*)malloc(sizeof(pageTable)*totalPageNum);


    for(int i=0;i<totalPageNum;++i)
        mainPageTable[i].PID = -1;

    CPUState = 0;

   CPUtimer = new QTimer(this);
   connect(CPUtimer, SIGNAL(timeout()), this, SLOT(handleCPUSlice()));
   CPUtimer->start(1000);


//    writeFile();

   //界面
   pro=new Dialog(this);
   Fpro=new File_Dialog(this);
   Cpro=new Create_Dialog(PIDmap,this);
   this->resize( QSize(1280,700));

   connect(this, SIGNAL(sendData(PCBList*,PCBList*,PCBList*,int,int,pageTable*)), pro, SLOT(open_pro(PCBList*,PCBList*,PCBList*,int,int,pageTable*)));
   connect(this, SIGNAL(sendData_PID(QMap<int, int>)), Cpro, SLOT(getPID(QMap<int, int>)));

   QPushButton* process=new QPushButton(this);
   QIcon computer(":/icon/timg.jpg");
   process->setIcon(computer);
   process->setIconSize(QSize(80,80));
   process->setFlat(true);
   process->setGeometry(QRect(QPoint(10, 80),QSize(75,75)));//设置位置和大小
   process->resize(QSize(75, 75));//重新设置大小

   QPushButton* file=new QPushButton(this);
   QIcon fi(":/icon/she.jpg");
   file->setIcon(fi);
   file->setIconSize(QSize(80,80));
   file->setFlat(true);
   file->setGeometry(QRect(QPoint(10, 180),QSize(75,75)));
   file->resize(QSize(75, 75));

   QPushButton* cre=new QPushButton(this);
   QIcon ce(":/icon/cre.jpg");
   cre->setIcon(ce);
   cre->setIconSize(QSize(80,80));
   cre->setFlat(true);
   cre->setGeometry(QRect(QPoint(10, 280),QSize(75,75)));
   cre->resize(QSize(75, 75));

   QPushButton* br=new QPushButton(this);
   QIcon b(":/icon/dyj.jpg");
   br->setIcon(b);
   br->setIconSize(QSize(80,80));
   br->setFlat(true);
   br->setGeometry(QRect(QPoint(10, 380),QSize(75,75)));
   br->resize(QSize(75, 75));


   connect(process, SIGNAL(clicked()), this, SLOT(on_process_clicked()));
   connect(file, SIGNAL(clicked()), this, SLOT(on_file_clicked()));
   connect(cre, SIGNAL(clicked()), this, SLOT(on_cre_clicked()));
   connect(br, SIGNAL(clicked()), this, SLOT(on_br_clicked()));

   QPixmap pixmap = QPixmap(":/main/tim.jpg").scaled(this->size());
   QPalette palette(this->palette());
   palette.setBrush(QPalette::Background, QBrush(pixmap));
   this->setPalette(palette);

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeStrategy(int pro,int page)
{
    curProStrategy = pro; //2-轮询 1-优先级 0-FCFS

    curPageStrategy = page;//0-FIFO,1-LRU
}

void MainWindow::writeFile()
{
    QFile file("data.txt");
    file.resize(0);
    QTextCodec *code=QTextCodec::codecForName("utf8");
    if (!file.open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text))
    {
        QMessageBox::critical(NULL, "提示", "无法创建文件");
    }
    else
    {
        QTextStream in(&file);
        in.setCodec(code);
        in<<"Men"<<4<<"\r\n";
        in<<"Num"<<1<<"\r\n";
        in<<"C"<<4<<"\r\n";
    }
}

void MainWindow::on_process_clicked()
{
//    emit sendData(waitingList,readyList,runningList);
    pro->show();

}

void MainWindow::on_file_clicked()
{
    Fpro->show();
}

void MainWindow::on_cre_clicked()
{
    Cpro->show();
}

void MainWindow::on_br_clicked()
{
    nowBreak = 1;
    PCBList* newPro = new PCBList();
    PCBList*runPro;

    int PID = 666,i;

    PIDmap.insert(PID,1);

    emit sendData_PID(PIDmap);

    //新建PCB
    newPro->next = NULL;

    newPro->process.pID = PID;
    newPro->process.prior = 0;
    newPro->process.comingTime = QDateTime::currentDateTime();
    newPro->process.curInstNeedTime = 0;
    newPro->process.curInstRunTime = 0;
    newPro->process.PC = 0;
    newPro->process.pState = 1;//ready

    //获取程序
    QTextCodec *code=QTextCodec::codecForName("utf8");
    QFile file("print.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(this,tr("Warning"),tr("打开文件失败"),QMessageBox::Yes);
        qDebug()<<"File could not be opened."<<endl;
    }
    else
    {
        QTextStream fp(&file);
        fp.setCodec(code);

        QString OC;
        int OP;
        fp>>OC>>OP;
        if(OC == "Mem")
        {
            newPro->process.pageNum = OP;
            newPro->process.page.resize(OP);
            for(i=0;i<OP;++i)
                newPro->process.page[i].resize(2);//物理页号+vailded

            //申请页表
//            int table[OP];
//            requestNewPage(OP,PID,table);
            for(i=0;i<OP;++i)
            {
                newPro->process.page[i][0] = -1;
                newPro->process.page[i][1] = 0;       //1-有效
            }
        }
        fp>>OC>>OP;
        if(OC == "Num")
        {
            newPro->process.instNum = OP;
            newPro->process.program.resize(OP);
            for(i=0;i<OP;++i)
                newPro->process.program[i].resize(2);//类型+时间 0-cpu,1-IO

            for(i=0;i<newPro->process.instNum;++i)
            {
                fp>>OC>>OP;
                if(OC == "C")
                {
                    newPro->process.program[i][0] = 0;
                    newPro->process.program[i][1] = OP;
                }
                else
                {
                    newPro->process.program[i][0] = 1;
                    newPro->process.program[i][1] = OP;
                }

            }

        }
    }


    runPro = runningList->next;
    if(runPro != NULL)
    {
        runningList->next = NULL;
        addReadyList(runPro);
    }
    runningList->next = newPro;
}

/*
 * 模块名称：terminalPro(int PID,int list)
 * 参数：PID:进程号，可能是执行完毕或者手动结束(从UI界面输入而得) list:当前要结束的进程所属的队列
 * 功能：结束进程进程号为PID的进程
 * 时间：2018.9.15
*/
void MainWindow::terminalPro(int PID,int list)
{
     int check = 0;          //跳出循环的一个标志
     PCBList* pcbList,*pre;

     switch(list)
     {
     case 0:
         pre = runningList;
         pcbList = runningList->next;
         break;
     case 1:
         pre = readyList;
         pcbList = readyList->next;
         break;
     case 2:
         pre = waitingList;
         pcbList = waitingList->next;
         break;
     }

     //遍历链表，根据PID找到要ter的进程
     while(check == 0&&pcbList != NULL)
     {
         if(pcbList->process.pID == PID)
         {
             //释放资源
             check = 1;
             pre->next = pcbList->next;

             QMap<int, int>::iterator it;  //QMap<int, int>用于ji进程管理界面的显示
             for(it=PIDmap.begin();it!=PIDmap.end();it++)
             {
                if(it.key() == PID) PIDmap.erase(it);  //QMap中去除该进程
             }

             emit sendData_PID(PIDmap);  //发信号，刷新进程管理UI

             for(int i=0;i<totalPageNum;++i)
             {
                 if(mainPageTable[i].PID == PID)
                 {
                     mainPageTable[i].PID = -1;
                 }
             }

             free(pcbList);  //释放该块占用的内存
         }
         else
         {//继续遍历
             pre = pcbList;
             pcbList = pcbList->next;
         }
     }

     //刷新页面,发信号
     emit sendData(waitingList,readyList,runningList,curProStrategy,curPageStrategy,mainPageTable);
}

/*
 * 函数名称：createPro(int PID, int prior, int fileIndex)
 * 函数功能：创建进程号为PID，优先级为prior的进程.目前支持五中进程创建
 * 参数含义：PID--进程标识信息  prior--进程优先级  fileIndex--UI点击传送的文件索引
 * 时间：2018.9.14
*/
void MainWindow::createPro(int PID, int prior, int fileIndex)
{
    int i;
    PCBList* newPro = new PCBList();
    QString fileName[5] ={"1.txt","2.txt","3.txt","4.txt","5.txt"};

    PIDmap.insert(PID,1);

    emit sendData_PID(PIDmap);

    //新建PCB
    newPro->next = NULL;

    newPro->process.pID = PID;
    newPro->process.prior = prior;
    newPro->process.comingTime = QDateTime::currentDateTime();
    newPro->process.curInstNeedTime = 0;
    newPro->process.curInstRunTime = 0;
    newPro->process.PC = 0;
    newPro->process.pState = 1;//ready

    //获取程序
    QTextCodec *code=QTextCodec::codecForName("utf8"); //在Qt中通过QTextCodec来调用setCodec()
    QFile file(":/"+fileName[fileIndex]);  //路径需对应
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))//只读，文本
    {
        QMessageBox::warning(this,tr("Warning"),tr("打开文件失败"),QMessageBox::Yes);
        qDebug()<<"File could not be opened."<<endl;
    }
    else
    {
        QTextStream fp(&file);
        fp.setCodec(code);

        QString OC;
        int OP;
        fp>>OC>>OP;
        if(OC == "Mem")
        {
            newPro->process.pageNum = OP;
            newPro->process.page.resize(OP);
            for(i=0;i<OP;++i)
                newPro->process.page[i].resize(2);//物理页号+vailded

            //申请页表
//            int table[OP];
//            requestNewPage(OP,PID,table);
            for(i=0;i<OP;++i)
            {
                newPro->process.page[i][0] = -1;      //-1，表示不在内存中，需调入
                newPro->process.page[i][1] = 0;       //1-有效
            }
        }
        fp>>OC>>OP;
        if(OC == "Num")
        {
            newPro->process.instNum = OP;
            printf("get instNum from txt = %d\n\n",OP);
            newPro->process.program.resize(OP);
            for(i=0;i<OP;++i)
                newPro->process.program[i].resize(2);//类型+时间 0-cpu,1-IO

            for(i=0;i<newPro->process.instNum;++i)
            {
                fp>>OC>>OP;
                if(OC == "C")
                {
                    newPro->process.program[i][0] = 0;
                    newPro->process.program[i][1] = OP;
                }
                else
                {
                    newPro->process.program[i][0] = 1;
                    newPro->process.program[i][1] = OP;
                }

            }

        }
    }
    //加入ready队列
    addReadyList(newPro);
}



void MainWindow::handleCPUSlice()
{
    int i,j,k=1;


    //遍历等待队列，更新ready队列
    i = searchWaitingList();

    //检查有无中断
    if(nowBreak == 1)
    {
        //运行中断队列的PCB
        j = runBreakProcess();
        if(j == 0)
        {
            //更新run队列
            j=(i);

            while(j == 0 && readyList->next != NULL)
            {
                j=processSchedule(i);
            }

            //运行run队列中的process
            if(runningList->next!=NULL)
            {
                runProcess();

            }
        }
    }
    else
    {
        //更新run队列
        j=processSchedule(i);
        while(j == 0 && readyList->next != NULL)
        {
            j=processSchedule(i);
        }

        //运行run队列中的process
        if(runningList->next!=NULL)
        {
            printf("\n111\n");
            runProcess();
        }
    }

    //刷新页面,发信号
    emit sendData(waitingList,readyList,runningList,curProStrategy,curPageStrategy,mainPageTable);

    CPUtimer->start(300);

}

int MainWindow::runBreakProcess()
{
    int phyPageNum,logicalPageNum;
    int instType,instTime;

    PCBList* runPro = runningList->next;
    if((runPro->process.curInstRunTime == 0 && runPro->process.curInstNeedTime == 0)) //需要重新取指令
    {
        //物理页号
        logicalPageNum = runPro->process.PC/2;
        if(runPro->process.page[logicalPageNum][1] == 1)
            phyPageNum = runPro->process.page[logicalPageNum][0];
        else
        {
            phyPageNum = requestNewPage_one(runPro->process.pID);
            runPro->process.page[logicalPageNum][1] = 1;
            runPro->process.page[logicalPageNum][0] = phyPageNum;
        }

        //判断物理页是否有效
        if(!isExitPage(runPro->process.pID,phyPageNum))
        {
            phyPageNum = requestNewPage_one(runPro->process.pID);
            runPro->process.page[logicalPageNum][1] = 1;
            runPro->process.page[logicalPageNum][0] = phyPageNum;
        }

        //取指
        instType = runPro->process.program[runPro->process.PC][0];
        instTime = runPro->process.program[runPro->process.PC][1];

        runPro->process.curInstNeedTime = instTime;
        runPro->process.curInstRunTime = 0;
        runPro->process.curInstRunTime ++;
        runPro->process.PC++;
    }
    else if(runPro->process.curInstRunTime == runPro->process.curInstNeedTime)
    {
        //释放资源
        QMap<int, int>::iterator it;
        for(it=PIDmap.begin();it!=PIDmap.end();it++)
        {
           if(it.key() == runPro->process.pID) PIDmap.erase(it);
        }

        emit sendData_PID(PIDmap);

        int PID = runPro->process.pID;
        for(int i=0;i<totalPageNum;++i)
        {
            if(mainPageTable[i].PID == PID)
            {
                mainPageTable[i].PID = -1;
            }
        }
        runningList->next = NULL;
        nowBreak = 0;
        return 0;
    }
    else
    {
        runPro->process.curInstRunTime ++;
    }
    return 1;
}

/*
 * 模块名称：runProcess()
 * 参数：----
 * 功能：进程执行
 * 时间：2018.9.20
*/

int MainWindow::runProcess()
{
    int phyPageNum,logicalPageNum;
    int instType,instTime,p;       //指令类型(CPU/IO),指令占用资源的时间

    PCBList* runPro = runningList->next;
    if(
       (runPro->process.curInstRunTime == runPro->process.curInstNeedTime
        && runPro->process.PC < runPro->process.instNum))     //需要重新取指令
    {
        printf("\n curInstRunTime=%d,curInstNeedTime=%d,PC=%d,instNum=%d\n",
               runPro->process.curInstRunTime,runPro->process.curInstNeedTime,
               runPro->process.PC,runPro->process.instNum);
        printf("\get inst\n");
        //物理页号
        logicalPageNum = runPro->process.PC/2;
        if(runPro->process.page[logicalPageNum][1] == 1)        //物理页号+vailded，1表示页有效
            phyPageNum = runPro->process.page[logicalPageNum][0];
        else
        {
            phyPageNum = requestNewPage_one(runPro->process.pID);
            runPro->process.page[logicalPageNum][1] = 1;
            runPro->process.page[logicalPageNum][0] = phyPageNum;
        }

        //判断物理页是否有效
        if(!isExitPage(runPro->process.pID,phyPageNum))
        {
            phyPageNum = requestNewPage_one(runPro->process.pID);
            runPro->process.page[logicalPageNum][1] = 1;
            runPro->process.page[logicalPageNum][0] = phyPageNum;
        }

        //取指
        instType = runPro->process.program[runPro->process.PC][0];
        instTime = runPro->process.program[runPro->process.PC][1];

        runPro->process.curInstNeedTime = instTime;
        runPro->process.curInstRunTime = 0;

        runPro->process.curInstRunTime ++;

        //所取指令类型为需IO资源，则需等待.
        if(instType == 1)
        {
            //移入wait
            addWaitingList(runPro);
            runningList->next = NULL;
            CPUState = 0;
        }

        runPro->process.PC++;

        printf("\n PC++ curInstRunTime=%d,curInstNeedTime=%d,PC=%d,instNum=%d\n",
               runPro->process.curInstRunTime,runPro->process.curInstNeedTime,
               runPro->process.PC,runPro->process.instNum);
    }
    else if(runPro->process.curInstRunTime == runPro->process.curInstNeedTime
            && runPro->process.PC == runPro->process.instNum)
    {
        printf("\ncurInstRunTime=%d,curInstNeedTime=%d,PC=%d,instNum=%d\n",
               runPro->process.curInstRunTime,runPro->process.curInstNeedTime,
               runPro->process.PC,runPro->process.instNum);
        //释放
        QMap<int, int>::iterator it;
        for(it=PIDmap.begin();it!=PIDmap.end();it++)
        {
           if(it.key() == runPro->process.pID) PIDmap.erase(it);
        }

        emit sendData_PID(PIDmap);

        for(int i=0;i<totalPageNum;++i)
        {
            if(mainPageTable[i].PID == runPro->process.pID)
            {
                mainPageTable[i].PID = -1;
            }
        }
        terminalPro(runPro->process.pID,0);
        free(runPro);
        printf("\n22\n");
        runningList->next = NULL;

    }
    else
    {
        logicalPageNum = (runPro->process.PC-1)/2;
        phyPageNum = runPro->process.page[logicalPageNum][0];
        //判断物理页是否有效
        if(!isExitPage(runPro->process.pID,phyPageNum))
        {
            phyPageNum = requestNewPage_one(runPro->process.pID);
            runPro->process.page[logicalPageNum][1] = 1;
            runPro->process.page[logicalPageNum][0] = phyPageNum;
        }
        runPro->process.curInstRunTime ++;
        printf("\n else : curInstRunTime=%d,curInstNeedTime=%d,PC=%d,instNum=%d\n",
               runPro->process.curInstRunTime,runPro->process.curInstNeedTime,
               runPro->process.PC,runPro->process.instNum);
    }

    return 1;
}


/*1.先判断run队列是否有进程。2.若无，根据调度策略，从ready中选择进程，注意判断ready中是否有进程。
 * 3.若有，根据调度策略进行不同的操作。4.若为轮询，则正常选择。5.若为FCFS，根据searchWaitingList（）的返回值，
 * 若为1，从ready中选择一个时间最早的进程与当前进程进行判断，在决定是否调换。6.若为优先级，类似
 * 返回值：run队列中是否发生的变化，1-有
 * 2-轮询 1-优先级 0-FCFS*/
int MainWindow::processSchedule(int change)
{
    //change没有用到
    PCBList* runPro;
    //选择run队列的进程
    if((runningList->next == NULL&&readyList->next != NULL)||(readyList->next != NULL))
    {
        runPro = runningList->next;
        if(runPro != NULL)
        {
            runningList->next = NULL;
            addReadyList(runPro);
        }


        switch(curProStrategy)
        {
        case 0:
            FIFS();
            break;
        case 1:
            PrioSchedule();
            break;
        case 2:
            TimeSchedule();
            break;
        }


        //判断所选的进程是否将要中止
        runPro = runningList->next;
        if(runPro->process.PC > runPro->process.instNum)
        {
            //释放内存
            QMap<int, int>::iterator it;
            for(it=PIDmap.begin();it!=PIDmap.end();it++)
            {
               if(it.key() ==runPro->process.pID) PIDmap.erase(it);
            }

            emit sendData_PID(PIDmap);

            int PID = runPro->process.pID;
            for(int i=0;i<totalPageNum;++i)
            {
                if(mainPageTable[i].PID == PID)
                {
                    mainPageTable[i].PID = -1;
                }
            }
            runningList->next = NULL;
            return 0;
        }
        if(runningList->next!=NULL)
            runPro->process.pState = 2;
    }

    return 1;


}


//确认页是否存在于内存,若存在，则返回1，且更新LRU。若不存在，则返回0
int MainWindow::isExitPage(int PID,int pageNum)
{
    if(mainPageTable[pageNum].PID == PID)
    {
        mainPageTable[pageNum].LRUTime = QDateTime::currentDateTime();
        return 1;
    }
    else
    {
        return 0;
    }
}

//申请新页
int MainWindow::requestNewPage(int pageNum, int PID, int* pageTable)  //pageNum>1，返回是否成功，1-ok
{
    int allotNum = 0;
    int i,j;

    for(i=0;i<totalPageNum && allotNum<pageNum ;++i)
    {
        if(mainPageTable[i].PID == -1)
        {
            pageTable[allotNum] = i;
            mainPageTable[i].PID = PID;
            mainPageTable[i].comingTime = QDateTime::currentDateTime();
            mainPageTable[i].LRUTime = QDateTime::currentDateTime();
            allotNum ++;
        }
    }

    while(allotNum<pageNum)
    {
        //启动换页算法
        switch(curPageStrategy)
        {
        case 0:
           i = FIFOPage();
           pageTable[allotNum] = i;
           mainPageTable[i].PID = PID;
           mainPageTable[i].comingTime = QDateTime::currentDateTime();
           mainPageTable[i].LRUTime = QDateTime::currentDateTime();
           allotNum ++;
           break;
        case 1:
            i = LRUpage();
            pageTable[allotNum] = i;
            mainPageTable[i].PID = PID;
            mainPageTable[i].comingTime = QDateTime::currentDateTime();
            mainPageTable[i].LRUTime = QDateTime::currentDateTime();
            allotNum ++;
            break;
        }
    }

    return 1;
}
int MainWindow::requestNewPage_one(int PID)  //pageNum=1,返回物理页号
{
    int pageNum = 1;
    int allotNum = 0;
    int i,j;

    for(i=0;i<totalPageNum && allotNum<pageNum ;++i)
    {
        if(mainPageTable[i].PID == -1)
        {
            mainPageTable[i].PID = PID;
            mainPageTable[i].comingTime = QDateTime::currentDateTime();
            mainPageTable[i].LRUTime = QDateTime::currentDateTime();
            allotNum ++;
            return i;
        }
    }

    if(allotNum<pageNum)
    {
        //启动换页算法
        switch(curPageStrategy)
        {
        case 0:
           i = FIFOPage();
           mainPageTable[i].PID = PID;
           mainPageTable[i].comingTime = QDateTime::currentDateTime();
           mainPageTable[i].LRUTime = QDateTime::currentDateTime();
           allotNum ++;
           break;
        case 1:
            i = LRUpage();
            mainPageTable[i].PID = PID;
            mainPageTable[i].comingTime = QDateTime::currentDateTime();
            mainPageTable[i].LRUTime = QDateTime::currentDateTime();
            allotNum ++;
            break;
        }
    }

    return i;
}
//获取某页的调入时间
QDateTime MainWindow::getPageComingTime(int num)
{
    return mainPageTable[num].comingTime;
}

//获取某页的LRUTIME
QDateTime MainWindow::getPageLRUTime(int num)
{
    return mainPageTable[num].LRUTime;
}


//换页算法
int MainWindow::LRUpage()
{
    int i;
    QDateTime min;
    int pageNum;

    min = mainPageTable[0].LRUTime;
    pageNum = 0;

    for(i=1;i<totalPageNum;++i)
    {
        if(mainPageTable[i].LRUTime<min)
        {
            min = mainPageTable[i].LRUTime;
            pageNum = i;
        }
    }

    return pageNum;
}

int MainWindow::FIFOPage()
{
    int i;
    QDateTime min;
    int pageNum;

    min = mainPageTable[0].comingTime;
    pageNum = 0;

    for(i=1;i<totalPageNum;++i)
    {
        if(mainPageTable[i].comingTime<min)
        {
            min = mainPageTable[i].comingTime;
            pageNum = i;
        }
    }

    return pageNum;
}

//调度
void MainWindow::PrioSchedule()
{
    PCBList *p = readyList->next, *pre = readyList;
    PCBList *minpre = pre, *minp = p;
    while(p!=NULL)
    {
        if(p->process.prior < minp->process.prior)
        {
            minp = p;
            minpre = pre;
        }
        pre = p;
        p = p->next;
    }
    runningList->next = minp;
    runningList->next->process.pState = 2;
    minpre->next = minp->next;
    runningList->next->next = NULL;
}

void MainWindow::FIFS()
{
    PCBList *p = readyList->next, *pre = readyList;
    PCBList *minpre = pre, *minp = p;
    while(p!=NULL)
    {
        if(p->process.comingTime < minp->process.comingTime)
        {
            minp = p;
            minpre = pre;
        }
        pre = p;
        p = p->next;
    }
    runningList->next = minp;
    minpre->next = minp->next;
    runningList->next->process.pState = 2;
    runningList->next->next = NULL;

}

void MainWindow::TimeSchedule()
{
    PCBList *p = readyList->next;
    if(p!=NULL)
    {
        runningList->next = p;
        readyList->next = readyList->next->next;
        runningList->next->next = NULL;
    }
}

int MainWindow::isRunListEmpty()
{
    if(runningList->next==NULL)
    {
        return 1;
    }
    else
        return 0;
}

int MainWindow::searchWaitingList()
{
    PCBList *p = waitingList->next, *pre = waitingList;
    int k = 0;
    while(p!=NULL)
    {
        p->process.curInstRunTime ++;   //---注释
//        qDebug()<<p->process.pID<<" "<<p->process.curInstRunTime<<" "<<p->process.curInstNeedTime<<endl;

//        if(p->process.pID == 2 && p->process.curInstRunTime == 5)
//            k = 0;
        if(p->process.curInstRunTime > p->process.curInstNeedTime)
        {
            printf("Why?");
            printf("\n curInstRunTime=%d,curInstNeedTime=%d,PC=%d,instNum=%d\n",
                   p->process.curInstRunTime,p->process.curInstNeedTime,p->process.PC,p->process.instNum);
            p->process.curInstRunTime = 0;
            p->process.curInstNeedTime = 0;

            pre->next = p->next;

            addReadyList(p);
            if(pre->next != NULL)
                p = pre->next->next;
            else
            {
                free(p);
                p = NULL;
            }

            k = 1;
        }
        else
        {
            pre = p;
            p = p->next;
            //p = p->next;

        }
        printf("why???\n");

    }
    return k;
}

int MainWindow::addWaitingList(PCBList* newPCB)
{
    PCBList *p = NULL, *s = NULL , *pre = NULL;
    s = newPCB;
    p = waitingList->next;
    pre = waitingList;
    while(p!=NULL)
    {
        pre = p;
        p = p->next;
    }
    pre->next = s;
    s->next = p;
    s->process.pState = 3;
    waitNum++;
    return 1;
}

int MainWindow::addReadyList(PCBList* newPCB)
{
    PCBList *p = NULL, *s, *pre = NULL;
    s = newPCB;
    p = readyList->next;
    pre = readyList;
    while(p!=NULL)
    {
        pre = p;
        p = p->next;
    }
    pre->next = s;
    s->next = NULL;
    s->process.pState = 1;
    readyNum++;
    return 1;
}
