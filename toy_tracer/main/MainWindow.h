#pragma once
#include <QtWidgets/QMainWindow>
#include <QTreeWidget>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
      Q_OBJECT
public slots:
      void importObj();
      void objLoadToggled(QTableWidgetItem* i);
      void refreshResource();
      void showProperties(QTableWidgetItem* obj, QTableWidgetItem* p);
protected:
      void mousePressEvent(QMouseEvent *ev) override;
      void mouseReleaseEvent(QMouseEvent *ev) override;
      void mouseMoveEvent(QMouseEvent *ev) override;

public:
      MainWindow(QWidget *parent = Q_NULLPTR);
      static MainWindow* getInstance();
private:
      bool _drag = false;
};