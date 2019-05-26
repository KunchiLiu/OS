#include "create_dialog.h"
#include "ui_create_dialog.h"
#include<QMessageBox>
#include<QString>
#include<qDebug>
Create_Dialog::Create_Dialog(QMap<int, int> PIDMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Create_Dialog)
{

    ui->setupUi(this);
    PIDmap_cre = PIDMap;

    this->resize( QSize(350,390));
    QPixmap pixmap = QPixmap(":/main/cre_b.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);

    ui->cbBox_filename->addItem(QWidget::tr("1.txt"));
    ui->cbBox_filename->addItem(QWidget::tr("2.txt"));
    ui->cbBox_filename->addItem(QWidget::tr("3.txt"));
    ui->cbBox_filename->addItem(QWidget::tr("4.txt"));
    ui->cbBox_filename->addItem(QWidget::tr("5.txt"));

    connect(this, SIGNAL(sendData(int, int , int )), parent, SLOT(createPro(int, int, int)));


}

Create_Dialog::~Create_Dialog()
{
    delete ui;
}

void Create_Dialog::on_pushButton_2_clicked()
{
    this->close();
}

void Create_Dialog::getPID(QMap<int, int> PIDMap)
{
    PIDmap_cre = PIDMap;
}

void Create_Dialog::on_pushButton_clicked()
{
    int PID,pritor,fileIndex;
    QString filename;


    bool ok;
    int check =0;
    PID = this->ui->create_PID->text().toInt(&ok,10);
    pritor = this->ui->create_Pritor->text().toInt(&ok,10);
//    filename = this->ui->create_filename->text();
    fileIndex = this->ui->cbBox_filename->currentIndex();

    QMap<int, int>::iterator it;
    for(it=PIDmap_cre.begin();it!=PIDmap_cre.end();it++)
    {
//        qDebug()<<it.key()<<endl;
       if(it.key() == PID)
       {
            QMessageBox::warning(this,tr("Warning"),tr("进程ID重复"),QMessageBox::Yes);
            check = 1;
       }
    }

    if(check == 0)
    {
        emit sendData(PID, pritor, fileIndex);
        this->close();
    }

}
