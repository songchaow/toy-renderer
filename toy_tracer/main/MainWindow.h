#pragma once
#include <QtWidgets/QMainWindow>
#include <QTreeWidget>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
      Q_OBJECT
private slots:
      void importObj();
      void objLoadToggled(QTreeWidgetItem* i);
      void refreshResource();
      void showProperties(QTableWidgetItem* obj);
public:
      MainWindow(QWidget *parent = Q_NULLPTR);
      static MainWindow* getInstance();
private:

};