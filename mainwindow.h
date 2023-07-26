#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <unistd.h>
#include "qhexedit.h"
#include "dialogsp.h"
extern "C" {
   #include "ch341a.h"
}


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    void receiveAddr(QString);
    void receiveAddr2(QString);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_size_currentIndexChanged(int index);

    void on_comboBox_page_currentIndexChanged(int index);

    void on_comboBox_speed_currentIndexChanged(int index);

    void on_actionDetect_triggered();

    void on_actionSave_triggered();

    void on_actionErase_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionOpen_triggered();

    void on_actionWrite_triggered();

    void on_actionRead_triggered();

    void on_actionExit_triggered();

    void on_comboBox_man_currentIndexChanged(int index);

    void on_comboBox_name_currentIndexChanged(const QString &arg1);

    void on_actionVerify_triggered();

    void on_pushButton_3_clicked();


    void on_actionSave_Part_triggered();



    void on_actionLoad_Part_triggered();

private:
    Ui::MainWindow *ui;
    int statusCH341;
    QByteArray chipData;
    uint32_t currentChipSize, currentNumBlocks, currentBlockSize;
    uint8_t currentSpeed;
    QString bytePrint(unsigned char z);
    QVector <QString> chType = {"SPI_FLASH","25_EEPROM","93_EEPROM","24_EEPROM","95_EEPROM"};
    struct chip_data {
        QString chipManuf;
        QString chipTypeTxt;
        QString chipName;
        uint8_t chipJedecIDMan;
        uint8_t chipJedecIDDev;
        uint8_t chipJedecIDCap;
        uint32_t chipSize;
        uint16_t blockSize;
        uint8_t chipTypeHex;
        uint8_t algorithmCode;
        int delay;
        uint8_t extend;
        uint8_t eeprom;
        uint8_t eepromPages;
        QString chipVCC;
    };
    chip_data chips[2000];
    int max_rec;
    QString fileName;
    QHexEdit *hexEdit;
    QString sizeConvert(int a);
    QString hexiAddr(int a);
    uint32_t hexToInt(QString str);
    QByteArray block;
    uint32_t blockStartAddr, blockLen;
};

#endif // MAINWINDOW_H
