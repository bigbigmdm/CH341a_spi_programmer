#ifndef DIALOGRP_H
#define DIALOGRP_H

#include <QDialog>
#include <QString>

namespace Ui {
class DialogRP;
}

class DialogRP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRP(QWidget *parent = nullptr);
    ~DialogRP();

private slots:
    void on_pushButton_clicked();

signals:
    void sendAddr2(QString);

private:
    Ui::DialogRP *ui;
    QString addrData;
};

#endif // DIALOGRP_H
