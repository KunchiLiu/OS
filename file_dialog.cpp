#include "file_dialog.h"
#include "ui_file_dialog.h"

File_Dialog::File_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::File_Dialog)
{
    ui->setupUi(this);

    this->resize( QSize(350,400));
    QPixmap pixmap = QPixmap(":/main/shezhi.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(QPalette::Background, QBrush(pixmap));
    this->setPalette(palette);

    ui->comboBox->addItem(QWidget::tr("先来先服务调度算法"));
    ui->comboBox->addItem(QWidget::tr("优先级调度算法"));
    ui->comboBox->addItem(QWidget::tr("时间片调度算法"));


    ui->comboBox2->addItem(QWidget::tr("FIFO"));
    ui->comboBox2->addItem(QWidget::tr("LRU"));

    connect(this, SIGNAL(sendData(int, int)), parent, SLOT(changeStrategy(int, int)));

}

File_Dialog::~File_Dialog()
{
    delete ui;
}

void File_Dialog::on_pushButton_2_clicked()
{
    this->close();
}

void File_Dialog::on_pushButton_clicked()
{
    int pro,page;

    pro = this->ui->comboBox->currentIndex();
    page = this->ui->comboBox2->currentIndex();
    emit sendData(pro,page);
    this->close();
}
