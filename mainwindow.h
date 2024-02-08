#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QStorageInfo>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString fileContent = "";
    int currentLetter = 0;

private slots:
    void on_pushButtonFindDrivers_clicked();

    void on_pushButtonOpenFile_clicked();

    void on_pushButtonShowData_clicked();

    void on_pushButtonNextSymbol_clicked();

    void on_pushButtonSaveFile_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
