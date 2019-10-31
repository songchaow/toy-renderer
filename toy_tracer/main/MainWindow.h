#pragma once
#include <QtWidgets/QMainWindow>
#include <QTreeWidget>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
      Q_OBJECT
public slots:
      void importObj();
      void addPointLight();
      void objLoadToggled(QTableWidgetItem* i);
      void refreshResource();
      void showProperties(QTableWidgetItem* obj, QTableWidgetItem* p);

public:
      MainWindow(QWidget *parent = Q_NULLPTR);
      static MainWindow* getInstance();

};