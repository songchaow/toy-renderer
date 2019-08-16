#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
      Q_OBJECT
private slots:
      void loadObj();
      void refreshResource();
      void showProperties(QTableWidgetItem* obj);
public:
      MainWindow(QWidget *parent = Q_NULLPTR);

private:

};