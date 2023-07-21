#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QErrorMessage>
#include <QtGui>
#include "qhexedit.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int index2;
    max_rec = 0;
    ui->setupUi(this);
    ui->statusBar->addPermanentWidget(ui->lStatus,0);
    ui->statusBar->addPermanentWidget(ui->eStatus,0);
    ui->statusBar->addPermanentWidget(ui->jLabel,0);
    ui->statusBar->addPermanentWidget(ui->jedecEdit,0);
    ui->progressBar->setValue(0);
    ui->comboBox_name->addItems({""});
    ui->comboBox_man->addItems({""});
    ui->comboBox_vcc->addItems({ " ", "3.3 V", "1.8 V", "5.0 V"});
    //0 = 20kHz; 1 = 100kHz, 2 = 400kHz, 3 = 750kHz
    //ui->comboBox_speed->addItem(" ", -1); //uncorrected value
    ui->comboBox_speed->addItem("750 khz", 3);
    ui->comboBox_speed->addItem("400 khz", 2);
    ui->comboBox_speed->addItem("100 khz", 1);
    ui->comboBox_speed->addItem("20 khz", 0);

    ui->comboBox_page->addItem(" ", 0);
    ui->comboBox_page->addItem("1", 1);
    ui->comboBox_page->addItem("2", 2);
    ui->comboBox_page->addItem("4", 4);
    ui->comboBox_page->addItem("8", 8);
    ui->comboBox_page->addItem("16", 16);
    ui->comboBox_page->addItem("32", 32);
    ui->comboBox_page->addItem("64", 64);
    ui->comboBox_page->addItem("128", 128);
    ui->comboBox_page->addItem("256", 256);
    ui->comboBox_page->addItem("512", 512);

    ui->comboBox_size->addItem(" ", 0);
    ui->comboBox_size->addItem("64 K", 64 * 1024);
    ui->comboBox_size->addItem("128 K", 128 * 1024);
    ui->comboBox_size->addItem("256 K", 256 * 1024);
    ui->comboBox_size->addItem("512 K", 512 * 1024);
    ui->comboBox_size->addItem("1 M", 1024 * 1024);
    ui->comboBox_size->addItem("2 M", 2048 * 1024);
    ui->comboBox_size->addItem("4 M", 4096 * 1024);
    ui->comboBox_size->addItem("8 M", 8192 * 1024);
    ui->comboBox_size->addItem("16 M", 16384 * 1024);
    ui->comboBox_size->addItem("32 M", 32768 * 1024);
    ui->comboBox_size->addItem("64 M", 65536 * 1024);
    currentChipSize = 0;
    currentNumBlocks = 0;
    currentBlockSize = 0;
    currentSpeed = 0;

    // connect and status check
    statusCH341 = ch341Configure(0x1a86,0x5512); // VID 1a86 PID 5512 for CH341A
    if (statusCH341 == 0)
    {
        ui->eStatus->setText("Connected");
        ui->eStatus -> setStyleSheet("QLineEdit {border: 2px solid gray;border-radius: 5px;color:#000;background:#9f0;font-weight:600;border-style:inset;}");
    }
    else
    {
        ui->eStatus->setText("Not connected");
        ui->eStatus -> setStyleSheet("QLineEdit {border: 2px solid gray;border-radius: 5px;color:#fff;background:#f00;font-weight:600;border-style:inset;}");
    }
    chipData.resize(256);
    for (int i=0; i < 256; i++)
    {
        chipData[i] = 0xff;
    }

    ch341SetStream(3);
    hexEdit = new QHexEdit(ui->frame);
    hexEdit->setGeometry(0,0,ui->frame->width(),ui->frame->height());
     hexEdit->setData(chipData);
    //ui->pushButton->resize(165, ui->pushButton->height()); //change height // widget->setFixedWidth(165)
    //opening chip database file
    ui->statusBar->showMessage("Opening DAT file");
    QFile datfile("CH341a.Dat");
    QByteArray dataChips;
    if (!datfile.open(QIODevice::ReadOnly))
    {
        QMessageBox::about(this, "Error", "Error loading chip database file!");
        return;
    }
    dataChips = datfile.readAll();
    datfile.close();
    //parsing dat file
    ui->statusBar->showMessage("Parsing DAT file");
    //parsing qbytearray
    char txtBuf[0x30];
    int i, j, recNo, dataPoz, dataSize, delay, rowCount, index;
    uint32_t chipSize;
    uint16_t blockSize;
    unsigned char chipSizeCode, chipID, manCode, tmpBuf;
    dataPoz = 0;
    recNo = 0;
    QStringList verticalHeader;
    dataSize = dataChips.length();
    while (dataPoz < dataSize)
    {
        for (j=0; j<0x30; j++)
             {
                 txtBuf[j] = 0;
             }
        j = 0;
             while ((j < 0x10) && (dataChips[recNo * 0x44 + j] != ',')) // ASCII data reading
             {
                 txtBuf[j] = dataChips[recNo * 0x44 + j];
                 j++;
             }
             if (txtBuf[1] == 0x00) break;
             chips[recNo].chipTypeTxt = QByteArray::fromRawData(txtBuf, 0x30);
         for (i=0; i<0x30; i++)
             {
                 txtBuf[i] = 0;
             }
         j++;
         i = 0;
         while ((i < 0x20) && (dataChips[recNo * 0x44 + j] != ',')) // ASCII data reading
         {
             txtBuf[i] = dataChips[recNo * 0x44 + j];
             j++;
             i++;
         }
             chips[recNo].chipManuf = QByteArray::fromRawData(txtBuf, 0x30);
             for (i=0; i<0x30; i++)
                 {
                     txtBuf[i] = 0;
                 }
             j++;
             i = 0;
             while ((i < 0x30) && (dataChips[recNo * 0x44 + j] != 0x00)) // ASCII data reading
             {
                 txtBuf[i] = dataChips[recNo * 0x44 + j];
                 j++;
                 i++;
             }
             chips[recNo].chipName = QByteArray::fromRawData(txtBuf, 0x30);            
             chips[recNo].chipJedecIDMan = dataChips[recNo * 0x44 + 0x32];
             chips[recNo].chipJedecIDDev = dataChips[recNo * 0x44 + 0x31];
             chips[recNo].chipJedecIDCap = dataChips[recNo * 0x44 + 0x30];
             tmpBuf = dataChips[recNo * 0x44 + 0x34];
             chipSize = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x35];
             chipSize = chipSize + tmpBuf * 256;
             tmpBuf = dataChips[recNo * 0x44 + 0x36];
             chipSize = chipSize + tmpBuf * 256 * 256;
             tmpBuf = dataChips[recNo * 0x44 + 0x37];
             chipSize = chipSize + tmpBuf * 256 * 256 * 256;
             chips[recNo].chipSize = chipSize;
             tmpBuf = dataChips[recNo * 0x44 + 0x38];
             blockSize = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x39];
             blockSize = blockSize + tmpBuf * 256;
             chips[recNo].blockSize = blockSize;
             tmpBuf = dataChips[recNo * 0x44 + 0x3a];
             chips[recNo].chipTypeHex = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x3b];
             chips[recNo].algorithmCode = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x3c];
             delay = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x3d];
             delay = delay + tmpBuf * 256;
             chips[recNo].delay = delay;
             tmpBuf = dataChips[recNo * 0x44 + 0x3e];
             chips[recNo].extend = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x40];
             chips[recNo].eeprom = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x42];
             chips[recNo].eepromPages = tmpBuf;
             tmpBuf = dataChips[recNo * 0x44 + 0x43];
             if (tmpBuf == 0x00) chips[recNo].chipVCC = "3.3 V";
             if (tmpBuf == 0x01) chips[recNo].chipVCC = "1.8 V";
             if (tmpBuf == 0x02) chips[recNo].chipVCC = "5.0 V";
             dataPoz = dataPoz + 0x44; //next record
             verticalHeader.append(QString::number(recNo));
             recNo++;
    }
    max_rec = recNo;
    for (i = 0; i<max_rec; i++)
    {
        //replacing items to combobox Manufacture
        index2 = ui->comboBox_man->findText(chips[i].chipManuf);
                    if ( index2 == -1 ) ui->comboBox_man->addItem(chips[i].chipManuf);
    }
     ui->comboBox_man->setCurrentIndex(0);
     ui->statusBar->showMessage("");

}

MainWindow::~MainWindow()
{
    delete ui;
}
QString MainWindow::bytePrint(unsigned char z)
{
    unsigned char s;
    s = z / 16;
    if (s > 0x9) s = s + 0x37;
    else s = s + 0x30;
    z = z % 16;
    if (z > 0x9) z = z + 0x37;
    else z = z + 0x30;
    return QString(s) + QString(z);
}

void MainWindow::on_pushButton_clicked()
{
    //Reading data from chip
    if ((currentNumBlocks > 0) && (currentBlockSize >0))
    {
    int addr = 0;
    int curBlock = 0;
    QString hexLine = "";
    QString asciiLine = "";
    int j, k, res;
    //progerssbar settings
    ui->progressBar->setRange(0, currentNumBlocks);
    ui->progressBar->setValue(0);
    uint8_t buf[currentBlockSize];
    ui->pushButton->setStyleSheet("QPushButton{color:#fff;background-color:#f66;border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
    ui->statusBar->showMessage("Reading data from " + ui->comboBox_name->currentText());
    //Select bus speed
    //
    for (k = 0; k < currentNumBlocks; k++)
      {
         //int32_t ch341SpiRead(uint8_t *buf, uint32_t add, uint32_t len);
         res = ch341SpiRead(buf, curBlock * currentBlockSize, currentBlockSize);
         // if res=-1 - error, stop
         if (statusCH341 != 0)
           {
             QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
             break;
           }
         if (res != 3)
           {
             QMessageBox::about(this, "Error", "Error reading sector " + QString::number(curBlock));
             break;
           }
         for (j = 0; j < currentBlockSize; j++)
            {
                  chipData[addr + j] = buf[addr + j - k * currentBlockSize];
            }
         addr = addr + currentBlockSize;
         curBlock++;
         hexEdit->setData(chipData);
         ui->progressBar->setValue(curBlock);
      }
    }
    else
    {
    //Not correct Number fnd size of blocks
     QMessageBox::about(this, "Error", "Before reading from chip please press 'Detect' button.");
    }
    ui->statusBar->showMessage("");
    ui->progressBar->setValue(0);
    ui->pushButton->setStyleSheet("QPushButton{color:#fff;background-color:rgb(120, 183, 140);border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
}
QString MainWindow::sizeConvert(int a)
{
    QString rez;
    rez = "0";
    if (a < 1024) rez = QString::number(a) + " B";
    else if ((a < 1024 * 1024)) rez = QString::number(a/1024) + " K";
    else rez = QString::number(a/1024/1024) + " M";
    return rez;
}

void MainWindow::on_pushButton_2_clicked()
{
    //searching the connected chip in database
    if (statusCH341 != 0)
      {
        QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
        return;
      }

    ui->pushButton_2->setStyleSheet("QPushButton {color:#fff;background-color:#f66;border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
    int i, index;
    // print JEDEC info
    struct JEDEC listJEDEC;
    //ui->sizeEdit->setText(QString::number(ch341SpiCapacity()));
    listJEDEC = GetJedecId();
    if ((listJEDEC.man == 0xff) && (listJEDEC.type == 0xff) && (listJEDEC.capacity == 0xff))
    {
        QMessageBox::about(this, "Error", "The chip is not connect or missing!");
        ui->pushButton_2->setStyleSheet("QPushButton{color:#fff;background-color:rgb(120, 183, 140);border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
        return;
    }
    ui->jedecEdit->setText(bytePrint(listJEDEC.man) + " " + bytePrint(listJEDEC.type) + " " + bytePrint(listJEDEC.capacity));
    for (i = 0; i< max_rec; i++)
    {
        if ((listJEDEC.man == chips[i].chipJedecIDMan) && (listJEDEC.type == chips[i].chipJedecIDDev) && (listJEDEC.capacity == chips[i].chipJedecIDCap))
        {            
            index = ui->comboBox_man->findText(chips[i].chipManuf);
                        if ( index != -1 )
                        { // -1 for not found
                           ui->comboBox_man->setCurrentIndex(index);
                        }
                        index = ui->comboBox_name->findText(chips[i].chipName);
                                    if ( index != -1 )
                                    { // -1 for not found
                                       ui->comboBox_name->setCurrentIndex(index);
                                    }

            index = ui->comboBox_size->findData(chips[i].chipSize);
            if ( index != -1 )
            { // -1 for not found
               ui->comboBox_size->setCurrentIndex(index);
            }
            index = ui->comboBox_page->findData(chips[i].blockSize);
            if ( index != -1 )
            { // -1 for not found
               ui->comboBox_page->setCurrentIndex(index);
            }
            index = ui->comboBox_vcc->findText(chips[i].chipVCC);
            if ( index != -1 )
            { // -1 for not found
               ui->comboBox_vcc->setCurrentIndex(index);
            }

            ui->pushButton_2->setStyleSheet("QPushButton{color:#fff;background-color:rgb(120, 183, 140);border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
            break;
        }
    }
    ui->pushButton_2->setStyleSheet("QPushButton{color:#fff;background-color:rgb(120, 183, 140);border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
}

void MainWindow::on_comboBox_size_currentIndexChanged(int index)
{
    //qDebug() <<"size="<< ui->comboBox_size->currentData().toInt() << " block_size=" << ui->comboBox_page->currentData().toInt();
    currentChipSize = ui->comboBox_size->currentData().toInt();
    currentBlockSize = ui->comboBox_page->currentData().toInt();
    if ((currentChipSize !=0) && (currentBlockSize!=0))
    {
        currentNumBlocks = currentChipSize / currentBlockSize;
        chipData.resize(currentChipSize);
        for (uint32_t i=0; i < currentChipSize; i++)
        {
            chipData[i] = 0xff;
        }
        hexEdit->setData(chipData);
    }
}

void MainWindow::on_comboBox_page_currentIndexChanged(int index)
{
    currentChipSize = ui->comboBox_size->currentData().toInt();
    currentBlockSize = ui->comboBox_page->currentData().toInt();
    if ((currentChipSize !=0) && (currentBlockSize!=0))
    {
        currentNumBlocks = currentChipSize / currentBlockSize;
        chipData.resize(currentChipSize);
        for (uint32_t i=0; i < currentChipSize; i++)
        {
            chipData[i] = 0xff;
        }
        hexEdit->setData(chipData);
    }
}

void MainWindow::on_comboBox_speed_currentIndexChanged(int index)
{
    if (ui->comboBox_speed->currentData().toInt() != -1)
    {
       currentSpeed = ui->comboBox_speed->currentData().toInt();
       ch341SetStream(currentSpeed);
    }
}

void MainWindow::on_actionDetect_triggered()
{
   MainWindow::on_pushButton_2_clicked();
}

void MainWindow::on_actionSave_triggered()
{

    ui->statusBar->showMessage("Saving file");
    fileName = QFileDialog::getSaveFileName(this,
                                QString::fromUtf8("Save file"),
                                QDir::currentPath(),
                                "Data Images (*.bin);;All files (*.*)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::about(this, "Error", "Error saving file!");
        return;
    }
    file.write(hexEdit->data());
    file.close();
}

void MainWindow::on_actionErase_triggered()
{
    int i;
    if (statusCH341 != 0)
      {
        QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
        return;
      }
    ui->statusBar->showMessage("Erasing the " + ui->comboBox_name->currentText());
    ui->checkBox->setStyleSheet("QCheckBox{font-weight:600;}");
    ui->centralWidget->repaint();
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(50);
    int res = ch341EraseChip();
    for (i=0; i < 120; i++)
    {
       sleep(1);
       qDebug() << "Status=" << ch341ReadStatus() << " " << i;
       if (ch341ReadStatus() == 0) break;
    }
    qDebug() << "Status=" << ch341ReadStatus();
    ui->checkBox->setStyleSheet("");
    ui->statusBar->showMessage("");
    ui->progressBar->setValue(0);
    ui->centralWidget->repaint();
}

void MainWindow::on_actionUndo_triggered()
{
    hexEdit->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    hexEdit->redo();
}

void MainWindow::on_actionOpen_triggered()
{
    ui->statusBar->showMessage("Opening file");
    fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Open file"),
                                QDir::currentPath(),
                                "Data Images (*.bin);;All files (*.*)");
    ui->statusBar->showMessage("Current file: " + fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {

        return;
    }
    chipData = file.readAll();
    hexEdit->setData(chipData);
    file.close();
    ui->statusBar->showMessage("");
}

void MainWindow::on_actionWrite_triggered()
{
    //Writting data to chip
    if (statusCH341 != 0)
      {
        QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
        return;
      }
    if ((currentNumBlocks > 0) && (currentBlockSize >0))
    {
    int addr = 0;
    int curBlock = 0;
    int j, k, res;
    ui->statusBar->showMessage("Writing data to " + ui->comboBox_name->currentText());
    //progerssbar settings
    ui->progressBar->setRange(0, currentNumBlocks);
    ui->checkBox_2->setStyleSheet("QCheckBox{font-weight:800;}");
    chipData = hexEdit->data();
    uint8_t buf[currentBlockSize];

    //Select bus speed
    //
    for (k = 0; k < currentNumBlocks; k++)
      {

         for (j = 0; j < currentBlockSize; j++)
            {
               buf[addr + j - k * currentBlockSize] = chipData[addr + j] ;
            }
         //int32_t ch341SpiWrite(uint8_t *buf, uint32_t add, uint32_t len)
         res = ch341SpiWrite(buf, curBlock * currentBlockSize, currentBlockSize);
         // if res=-1 - error, stop
         if (statusCH341 != 0)
           {
             QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
             break;
           }
         if (res != 0)
           {
             QMessageBox::about(this, "Error", "Error writing sector " + QString::number(curBlock));
             break;
           }
         addr = addr + currentBlockSize;
         curBlock++;
         ui->progressBar->setValue(curBlock);

      }
    }
    else
    {
    //Not correct Number fnd size of blocks
     QMessageBox::about(this, "Error", "Before reading from chip please press 'Detect' button.");
    }
    ui->progressBar->setValue(0);
    ui->checkBox_2->setStyleSheet("");
}

void MainWindow::on_actionRead_triggered()
{
    MainWindow::on_pushButton_clicked();
}

void MainWindow::on_actionExit_triggered()
{
    ch341Release();
    MainWindow::close();
}

void MainWindow::on_comboBox_man_currentIndexChanged(int index)
{
    int i, index2;
    QString txt="";
    if (max_rec > 0)
    {
       txt = ui->comboBox_man->currentText().toUtf8();
       ui->comboBox_name->clear();
       ui->comboBox_name->addItem("");
       for (i = 0; i<max_rec; i++)
       {
           //replacing items to combobox chip Name
           if (txt.compare(chips[i].chipManuf)==0)
           {
           index2 = ui->comboBox_name->findText(chips[i].chipName);
                    if ( index2 == -1 ) ui->comboBox_name->addItem(chips[i].chipName);
           }
       }
        ui->comboBox_name->setCurrentIndex(0);
        ui->statusBar->showMessage("");
   }

}

void MainWindow::on_comboBox_name_currentIndexChanged(const QString &arg1)
{
    int i, index;
    QString manName = ui->comboBox_man->currentText();
    if (arg1.compare("") !=0)
    {

       for (i = 0; i < max_rec; i++)
       {
           if ((manName.compare(chips[i].chipManuf)==0) && (arg1.compare(chips[i].chipName)==0))
           {
               index = ui->comboBox_size->findData(chips[i].chipSize);
               if ( index != -1 )
               { // -1 for not found
                  ui->comboBox_size->setCurrentIndex(index);
               }
               index = ui->comboBox_page->findData(chips[i].blockSize);
               if ( index != -1 )
               { // -1 for not found
                  ui->comboBox_page->setCurrentIndex(index);
               }
               index = ui->comboBox_vcc->findText(chips[i].chipVCC);
               if ( index != -1 )
               { // -1 for not found
                  ui->comboBox_vcc->setCurrentIndex(index);
               }
           }
       }

    }
}

void MainWindow::on_actionVerify_triggered()
{
    //Reading data from chip
    if ((currentNumBlocks > 0) && (currentBlockSize >0))
    {
    int addr = 0;
    int curBlock = 0;
    QString hexLine = "";
    QString asciiLine = "";
    int i,j, k, res;
    //progerssbar settings
    ui->progressBar->setRange(0, currentNumBlocks);
    ui->progressBar->setValue(0);
    uint8_t buf[currentBlockSize];
    chipData = hexEdit->data();
    ui->checkBox_3->setStyleSheet("QCheckBox{font-weight:800;}");
    ui->statusBar->showMessage("Veryfing data from " + ui->comboBox_name->currentText());
    //Select bus speed
    //
    for (k = 0; k < currentNumBlocks; k++)
      {
         //int32_t ch341SpiRead(uint8_t *buf, uint32_t add, uint32_t len);
         res = ch341SpiRead(buf, curBlock * currentBlockSize, currentBlockSize);
         // if res=-1 - error, stop
         if (statusCH341 != 0)
           {
             QMessageBox::about(this, "Error", "Programmer CH341a is not connected!");
             break;
           }
         if (res != 3)
           {
             QMessageBox::about(this, "Error", "Error reading sector " + QString::number(curBlock));
             break;
           }
         for (j = 0; j < currentBlockSize; j++)
            {
                 if (chipData[addr + j] != buf[addr + j - k * currentBlockSize])
                 {
                     //error compare
                     QMessageBox::about(this, "Error", "Error comparing data!\nAddress:   " + hexiAddr(addr + j) + "\nBuffer: " + bytePrint(chipData[addr + j]) + "    Chip: " + bytePrint(buf[addr + j - k * currentBlockSize]));
                     ui->statusBar->showMessage("");
                     ui->checkBox_3->setStyleSheet("");
                     return;
                 }
            }
         addr = addr + currentBlockSize;
         curBlock++;
         ui->progressBar->setValue(curBlock);
      }
    }
    else
    {
    //Not correct Number fnd size of blocks
     QMessageBox::about(this, "Error", "Before reading from chip please press 'Detect' button.");
    }
    ui->statusBar->showMessage("");
    ui->progressBar->setValue(0);
    ui->checkBox_3->setStyleSheet("");
}
QString MainWindow::hexiAddr(int add)
{
 QString rez = "";
 uint8_t A,B,C,D;
 D = 0xFF & add;
 add = add >> 8;
 C = 0xFF & add;
 add = add >> 8;
 B = 0xFF & add;
 add = add >> 8;
 A = 0xFF & add;
 rez = bytePrint(A) + bytePrint(B) + bytePrint(C) + bytePrint(D);
 return rez;
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->pushButton_3->setStyleSheet("QPushButton{color:#fff;background-color:#f66;border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
    if (ui->checkBox->isChecked()) MainWindow::on_actionErase_triggered();
    if (ui->checkBox_2->isChecked()) MainWindow::on_actionWrite_triggered();
    if (ui->checkBox_3->isChecked()) MainWindow::on_actionVerify_triggered();
    ui->pushButton_3->setStyleSheet("QPushButton{color:#fff;background-color:rgb(120, 183, 140);border-radius: 20px;border: 2px solid #094065;border-radius:8px;font-weight:600;}");
}

