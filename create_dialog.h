#ifndef CREATE_DIALOG_H
#define CREATE_DIALOG_H

#include <QDialog>
#include<QStandardItemModel>
namespace Ui {
class Create_Dialog;
}

class Create_Dialog : public QDialog
{
    Q_OBJECT
signals:
    void  sendData(int PID, int prior, int fileName);
public:
    explicit Create_Dialog(QMap<int, int>,QWidget *parent = 0);
    ~Create_Dialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void getPID(QMap<int, int>);

private:
    Ui::Create_Dialog *ui;
    QMap<int, int> PIDmap_cre;
};

#endif // CREATE_DIALOG_H
