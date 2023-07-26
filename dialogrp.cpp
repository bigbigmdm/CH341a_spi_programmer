#include "dialogrp.h"
#include "ui_dialogrp.h"
#include "mainwindow.h"
#include <QValidator>
#include <QRegExp>
#include <QDebug>
#include <QString>
DialogRP::DialogRP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRP)
{
    ui->setupUi(this);
    QRegExp reHex( "[A-Fa-f0-9]{1,8}" );
    QRegExpValidator *validator = new QRegExpValidator(reHex, this);
    ui->lineEditStart->setValidator(validator);
}

DialogRP::~DialogRP()
{
    delete ui;
}

void DialogRP::on_pushButton_clicked()
{

    addrData = QString(ui->lineEditStart->text());
    emit sendAddr2(addrData);
    DialogRP::close();
}
