#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
      Q_OBJECT

public:
      MainWindow(QWidget *parent = Q_NULLPTR) { ui.setupUi(this); }

private:
      Ui::MainWindow ui;
};