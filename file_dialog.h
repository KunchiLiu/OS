#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <QDialog>

namespace Ui {
class File_Dialog;
}

class File_Dialog : public QDialog
{
    Q_OBJECT
signals:
    void  sendData(int pro, int page);

public:
    explicit File_Dialog(QWidget *parent = 0);
    ~File_Dialog();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::File_Dialog *ui;
};

#endif // FILE_DIALOG_H
