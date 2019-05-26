#include "dialog.h"
#include "ui_dialog.h"
#include<QStandardItemModel>
#include<QPainter>
#include<QMessageBox>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    this->resize( QSize(800,500));
    QPixmap pixmap = QPixmap(":/main/pro2.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);



    waitingList = new PCBList();
    waitingList->next = NULL;

    readyList = new PCBList();
    readyList->next = NULL;

    runningList = new PCBList();
    runningList->next = NULL;

  connect(this,SIGNAL(senData_terminalPro(int,int)), parent, SLOT(terminalPro(int,int)));




}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    this->close();
}

void Dialog:: open_pro(PCBList* waitingLists, PCBList* readyLists, PCBList* runningLists, int curProStrategy, int curPageStrategy, pageTable* mainPageTable)
{
    QString strategyPro[3]={"FCFS" , "优先级调度", "轮转调度"};
    QString strategyPage[2]={"FIFO" , "LRU"};
    int cpuUse=0;
    QString cpuUseString;
    this->waitingList = waitingLists;
    this->readyList = readyLists;
    this->runningList = runningLists;

    ui->tableView->clearSpans();
    QStandardItemModel  *model = new QStandardItemModel();

     model->setColumnCount(8);
     model->setHeaderData(0,Qt::Horizontal,QString::fromUtf8("进程ID"));
     model->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("状态"));
     model->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("优先级"));
     model->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("总指令数"));
     model->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("PC"));
     model->setHeaderData(5,Qt::Horizontal,QString::fromUtf8("已运行时间"));
     model->setHeaderData(6,Qt::Horizontal,QString::fromUtf8("总时间"));
     model->setHeaderData(7,Qt::Horizontal,QString::fromUtf8("占用的物理页号"));
     ui->tableView->setModel(model);

        //表头信息显示居左

     ui->tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
     ui->tableView->verticalHeader()->hide();

        //设置列宽不可变

      ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(6,QHeaderView::Fixed);
      ui->tableView->horizontalHeader()->setSectionResizeMode(7,QHeaderView::Fixed);
      ui->tableView->setColumnWidth(0,80);
      ui->tableView->setColumnWidth(1,80);
      ui->tableView->setColumnWidth(2,80);
      ui->tableView->setColumnWidth(3,100);
      ui->tableView->setColumnWidth(4,100);
      ui->tableView->setColumnWidth(5,100);
      ui->tableView->setColumnWidth(6,100);
      ui->tableView->setColumnWidth(7,100);

      int i=0;
      PCBList* pcb;
      QString phyPage = "";

      pcb = runningList->next;
      while(pcb!=NULL){
         model->setItem(i,0,new QStandardItem(QString::number(pcb->process.pID)));
         model->setItem(i,1,new QStandardItem(QString::fromUtf8("running")));
         model->setItem(i,2,new QStandardItem(QString::number(pcb->process.prior)));
         model->setItem(i,3,new QStandardItem(QString::number(pcb->process.instNum)));
         model->setItem(i,4,new QStandardItem(QString::number(pcb->process.PC)));
         model->setItem(i,5,new QStandardItem(QString::number(pcb->process.curInstRunTime)));
         model->setItem(i,6,new QStandardItem(QString::number(pcb->process.curInstNeedTime)));

         phyPage = "";
         for(int i = 0;i<pcb->process.pageNum;++i)
         {
             if(pcb->process.page[i][1] == 1){
                 if(mainPageTable[pcb->process.page[i][0]].PID == pcb->process.pID){
                    cpuUse++;
                    phyPage = phyPage+ QString::number(pcb->process.page[i][0],10)+" " ;}}
         }
         model->setItem(i,7,new QStandardItem(phyPage));
         for(int k=0;k<8;k++){
             model->item(i,k)->setTextAlignment(Qt::AlignCenter);
         }
         i++;
         pcb=pcb->next;
      }

      pcb = readyList->next;
      while(pcb!=NULL){

         model->setItem(i,0,new QStandardItem(QString::number(pcb->process.pID)));
         model->setItem(i,1,new QStandardItem(QString::fromUtf8("ready")));
         model->setItem(i,2,new QStandardItem(QString::number(pcb->process.prior)));
         model->setItem(i,3,new QStandardItem(QString::number(pcb->process.instNum)));
         model->setItem(i,4,new QStandardItem(QString::number(pcb->process.PC)));
         model->setItem(i,5,new QStandardItem(QString::number(pcb->process.curInstRunTime)));
         model->setItem(i,6,new QStandardItem(QString::number(pcb->process.curInstNeedTime)));
         phyPage = "";
         for(int i = 0;i<pcb->process.pageNum;++i)
         {
             if(pcb->process.page[i][1] == 1){
                 if(mainPageTable[pcb->process.page[i][0]].PID == pcb->process.pID){
                    cpuUse++;
                    phyPage = phyPage+ QString::number(pcb->process.page[i][0],10)+" " ;}}
         }
         model->setItem(i,7,new QStandardItem(phyPage));
         for(int k=0;k<8;k++){
             model->item(i,k)->setTextAlignment(Qt::AlignCenter);
         }
         i++;
         pcb=pcb->next;
      }

      pcb = waitingList->next;
      while(pcb!=NULL){

         model->setItem(i,0,new QStandardItem(QString::number(pcb->process.pID)));
         model->setItem(i,1,new QStandardItem(QString::fromUtf8("waiting")));
         model->setItem(i,2,new QStandardItem(QString::number(pcb->process.prior)));
         model->setItem(i,3,new QStandardItem(QString::number(pcb->process.instNum)));
         model->setItem(i,4,new QStandardItem(QString::number(pcb->process.PC)));
         model->setItem(i,5,new QStandardItem(QString::number(pcb->process.curInstRunTime)));
         model->setItem(i,6,new QStandardItem(QString::number(pcb->process.curInstNeedTime)));
         phyPage = "";
         for(int i = 0;i<pcb->process.pageNum;++i)
         {
             if(pcb->process.page[i][1] == 1){
                 if(mainPageTable[pcb->process.page[i][0]].PID == pcb->process.pID){
                    cpuUse++;
                    phyPage = phyPage+ QString::number(pcb->process.page[i][0],10)+" " ;}}
         }
         model->setItem(i,7,new QStandardItem(phyPage));
         for(int k=0;k<8;k++){
             model->item(i,k)->setTextAlignment(Qt::AlignCenter);
         }
         i++;
         pcb=pcb->next;
      }

      if(cpuUse<=10)
      {
          cpuUse = cpuUse*10;
          cpuUseString = QString::number(cpuUse,10)+"%";
          this->ui->labelCpu->setText(cpuUseString);
      }

      this->ui->labelProSchedule->setText(strategyPro[curProStrategy]);
      this->ui->labelPageSchedule->setText(strategyPage[curPageStrategy]);

}




//void Dialog::on_pushButton_2_clicked()
//{

//   //emit sent(waitingList,readyList,running);
//}

void Dialog::on_btnStopPro_clicked()
{
    int check =0,PID,list = 0;
    PCBList* pcb;
    bool ok;
    PID = this->ui->stopPID->text().toInt(&ok,10);

    pcb = runningList->next;
    while(check==0 && pcb!=NULL){
        if(pcb->process.pID == PID){
            list = 0;
            check =1;}
        else
            pcb=pcb->next;
    }

    pcb = readyList->next;
    while(check==0 && pcb!=NULL){
        if(pcb->process.pID == PID){
            list = 1;
            check =1;}
        else
            pcb=pcb->next;
    }

    pcb = waitingList->next;
    while(check==0 && pcb!=NULL){
        if(pcb->process.pID == PID){
            list = 2;
            check =1;}
        else
            pcb=pcb->next;
    }

    if(check == 1)
        emit senData_terminalPro(PID,list);
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("当前无此进程"),QMessageBox::Yes);
    }

}
