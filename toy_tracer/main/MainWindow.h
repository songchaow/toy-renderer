#pragma once
#include <QtWidgets/QMainWindow>
#include <QTreeWidget>
#include "main/scene_object.h"
#include "light/point.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
      Q_OBJECT
            
      RendererObject* _currobj = nullptr;
      QWidget* property_container;
private slots:
      void viewToggled(bool checked);
public slots:
      void importObj();
      void addPointLight();
      void objLoadToggled(QTableWidgetItem* i);
      void refreshResource();
      void sceneItemClicked(QTableWidgetItem* obj, QTableWidgetItem* p);

public:
      void addPointLight(PointLight* pl);
      MainWindow(QWidget *parent = Q_NULLPTR);
      static MainWindow* getInstance();
      RendererObject* getCurrentItem() const { return _currobj; }

};