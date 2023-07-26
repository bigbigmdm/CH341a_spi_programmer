#ifndef DIALOGSP_H
#define DIALOGSP_H

#include <QDialog>
#include <QString>

namespace Ui {
class DialogSP;
}

class DialogSP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSP(QWidget *parent = nullptr);
    ~DialogSP();

private slots:
    void on_pushButton_clicked();

signals:
    void sendAddr(QString);

private:
    Ui::DialogSP *ui;
    QString addrData;
};

#endif // DIALOGSP_H
