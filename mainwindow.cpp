#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButtonFindDrivers_clicked()
{
    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes(); // list of drivers
    QList<QStorageInfo> usbDrives;

    for (const QStorageInfo& storage : storageList) { // filter list
        if (storage.fileSystemType() == "exFAT" || storage.fileSystemType() == "vFAT" || storage.fileSystemType() == "FAT32") {
            usbDrives.append(storage);
        }
    }

    ui->comboBoxDrivers->clear();
    foreach (const QStorageInfo &storage, usbDrives) { // add all USB drivers
        ui->comboBoxDrivers->addItem(storage.rootPath());
    }
}



void MainWindow::on_pushButtonOpenFile_clicked()
{
    QList<QStorageInfo> storageList = QStorageInfo::mountedVolumes(); // list of drivers

    QString selectedStorageName = ui->comboBoxDrivers->currentText(); // get current driver letter

    QStorageInfo selectedStorage;
    foreach (const QStorageInfo &storage, storageList) { // set current driver because its letter
        if (storage.rootPath() == selectedStorageName) {
            selectedStorage = storage;
            break;
        }
    }

    // set driver information in labels
    ui->labelName->setText(selectedStorage.name());
    long long memoryFree = selectedStorage.bytesFree() / (1024 * 1024);
    ui->labelFreeSpace->setText(QString::number(memoryFree) + " MB");
    long long memoryTotal = selectedStorage.bytesTotal() / (1024 * 1024);
    ui->labelCapacity->setText(QString::number(memoryTotal) + " MB");
    ui->labelType->setText(selectedStorage.fileSystemType());
    ui->labelAddress->setText(selectedStorage.rootPath());

    // set driver letter in file locations
    ui->lineEditOpenFileLocation->setText(selectedStorageName);
    ui->lineEditSaveFileLocation->setText(selectedStorageName);
}



void MainWindow::on_pushButtonShowData_clicked()
{
    QString filePath = ui->lineEditOpenFileLocation->text(); // get file path from label
    QFile file(filePath); // create object file

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { // try to open file
        if(file.size() == 0) { // if file open but it is empty
            ui->lineEditOutputData->setStyleSheet("color: rgb(255, 0, 0);background-color: rgb(255, 255, 255);");
            ui->lineEditOutputData->setText("ERROR! FILE IS EMPTY!");
        } else { // if file open
            ui->lineEditOutputData->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(255, 255, 255);"); // return black color for text after misstake
            ui->labelSymbol->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(185, 185, 185);");
            QTextStream in(&file); // create text reader from file

            in.setCodec("UTF-8"); // utf-8

            fileContent = in.readAll(); // read all data from file

            ui->lineEditOutputData->setText(fileContent); // set data to label

            ui->labelSymbol->setText(fileContent.at(0)); // set first letter to label

            QTextCodec *codec = QTextCodec::codecForName("CP866"); // codec
            QByteArray byteArray = codec->fromUnicode(fileContent); // byte array from letter codes mb
            int byte = byteArray.at(0); // get first element
            QString ascii = QString("%1").arg(QString::number(byte, 2), 8, '0').right(8); // make ascii 8bit code
            ui->labelASCII->setText(ascii); // set ascii code to label

            QString nrzi;
            if (ascii.at(0) == "1") { // if first ascii num == 1
                int temp = 0; // set temp 0
                nrzi = "0"; // set first nrzi num 0
                for (int i = 1; i < 8; i++) { // for other 7 bits
                    if(ascii.at(i) == "1") { // if ascii num == 1
                        nrzi += QString::number(temp); // set nrzi num same to previous
                    } else { // else if ascii num == 0
                        if (temp == 0) // invert temp
                            temp++;
                        else
                            temp--;
                        nrzi += QString::number(temp); // and set it
                    }
                }
            } else { // else if first ascii num == 1
                int temp = 1; // set temp 1
                nrzi = "1"; // set first nrzi num 1
                for (int i = 1; i < 8; i++) { // for other 7 bits
                    if(ascii.at(i) == "1") { // if ascii num == 1
                        nrzi += QString::number(temp); // set nrzi num same to previous
                    } else { // else if ascii num == 0
                        if (temp == 0) // invert temp
                            temp++;
                        else
                            temp--;
                        nrzi += QString::number(temp); // and set it
                    }
                }
            }

            ui->labelNRZI->setText(nrzi); // set nrzi codeto label

            file.close(); // close file
        }
    } else { // else if path to file is wrong
        ui->lineEditOutputData->setStyleSheet("color: rgb(255, 0, 0);background-color: rgb(255, 255, 255);");
        ui->lineEditOutputData->setText("ERROR! FILE IS NOT FOUND!");
    }
}



void MainWindow::on_pushButtonNextSymbol_clicked()
{
    if (fileContent == "") { // if file is empty
        ui->labelSymbol->setStyleSheet("color: rgb(255, 0, 0);background-color: rgb(185, 185, 185);");
        ui->labelSymbol->setText("ERROR!");
    } else {
        ui->labelSymbol->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(185, 185, 185);"); // return black color for text after misstake

        currentLetter++;
        if (currentLetter == fileContent.length()) {
            currentLetter = 0;
        }

        ui->labelSymbol->setText(fileContent.at(currentLetter)); // set first letter to label

        QTextCodec *codec = QTextCodec::codecForName("CP866"); // codec
        QByteArray byteArray = codec->fromUnicode(fileContent); // byte array from letter codes mb
        int byte = byteArray.at(currentLetter); // get current element
        QString ascii = QString("%1").arg(QString::number(byte, 2), 8, '0').right(8); // make ascii 8bit code
        ui->labelASCII->setText(ascii); // set ascii code to label

        QString nrzi;
        if (ascii.at(0) == "1") { // if first ascii num == 1
            int temp = 0; // set temp 0
            nrzi = "0"; // set first nrzi num 0
            for (int i = 1; i < 8; i++) { // for other 7 bits
                if(ascii.at(i) == "1") { // if ascii num == 1
                    nrzi += QString::number(temp); // set nrzi num same to previous
                } else { // else if ascii num == 0
                    if (temp == 0) // invert temp
                        temp++;
                    else
                        temp--;
                    nrzi += QString::number(temp); // and set it
                }
            }
        } else { // else if first ascii num == 1
            int temp = 1; // set temp 1
            nrzi = "1"; // set first nrzi num 1
            for (int i = 1; i < 8; i++) { // for other 7 bits
                if(ascii.at(i) == "1") { // if ascii num == 1
                    nrzi += QString::number(temp); // set nrzi num same to previous
                } else { // else if ascii num == 0
                    if (temp == 0) // invert temp
                        temp++;
                    else
                        temp--;
                    nrzi += QString::number(temp); // and set it
                }
            }
        }

        ui->labelNRZI->setText(nrzi); // set nrzi code to label
    }


}



void MainWindow::on_pushButtonSaveFile_clicked()
{
    QString dataString = ui->lineEditOutputData->text(); // data
    QString filePath = ui->lineEditSaveFileLocation->text(); // save data in file

    QFile file(filePath); // create object file

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { // try to open file
        ui->lineEditOutputData->setStyleSheet("color: rgb(0, 0, 0);background-color: rgb(255, 255, 255);"); // return black color for text after misstake
        QTextStream stream(&file);
        stream.setCodec("UTF-8"); // utf-8
        stream << dataString; // write data to file
        file.close();
    } else { // else if file is not found
        ui->lineEditOutputData->setStyleSheet("color: rgb(255, 0, 0);background-color: rgb(255, 255, 255);");
        ui->lineEditOutputData->setText("ERROR! FILE IS NOT FOUND!");
    }

    // clear all widgets
    ui->comboBoxDrivers->clear();

    ui->lineEditOpenFileLocation->clear();
    ui->lineEditSaveFileLocation->clear();
    ui->lineEditOutputData->clear();

    ui->labelName->clear();
    ui->labelFreeSpace->clear();
    ui->labelCapacity->clear();
    ui->labelType->clear();
    ui->labelAddress->clear();

    ui->labelSymbol->clear();
    ui->labelASCII->clear();
    ui->labelNRZI->clear();

    // clear variables
    fileContent = "";
    currentLetter = 0;
}
