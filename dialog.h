#ifndef DIALOG_H
#define DIALOG_H
#include<QStandardItemModel>
#include <QDialog>
#include<QDateTime>
#include<define.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

    ~Dialog();
signals:
    void sent(PCBList* waitingList,PCBList* readyList,PCBList* runningList);
    void senData_terminalPro(int,int);
private slots:
    void on_pushButton_clicked();
    //void on_pushButton_2_clicked();
   // void re_paint(QStandardItemModel *model);
    void open_pro(PCBList* waitingList,PCBList* readyList,PCBList* runningList,int,int,pageTable*);
    void on_btnStopPro_clicked();

private:
    Ui::Dialog *ui;
    PCBList* waitingList;
    PCBList* readyList;
    PCBList* runningList;

};

#endif // DIALOG_H
