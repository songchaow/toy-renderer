#include "main/MainWindow.h"
#include "core/ResourceManager.h"
#include "main/renderworker.h"
#include <QFileDialog>

MainWindow* _mainWindow;

MainWindow::MainWindow(QWidget *parent/* = Q_NULLPTR*/) {
      _mainWindow = this;
      setupUi(this);
      resourceWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      QObject::connect(loadResButton, SIGNAL(clicked()), this, SLOT(importObj()));
      QObject::connect(resourceWidget, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem *)), this, SLOT(showProperties(QTableWidgetItem* ,QTableWidgetItem *)));
      QObject::connect(resourceWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(objLoadToggled(QTableWidgetItem *)));
}

void MainWindow::showProperties(QTableWidgetItem* obj, QTableWidgetItem* p) {
      RendererObject* robj = static_cast<RendererObject*>(obj->data(Qt::UserRole).value<void*>());
      if (robj) {
            robj->addProperties(properties);
            if (robj->typeID() == RendererObject::TypeID::Primitive) {
                  PBRMaterial* m = static_cast<Primitive*>(robj)->getPBRMaterial();
                  if (m)
                        m->addProperties(materialWidget);

            }
      }
}

void MainWindow::importObj() {
      QFileDialog dialog(this, "Load Resource...", QString(), ResourceManager::getInstance()->filters.join(';'));
      dialog.exec();
      resourcePath->setText(dialog.selectedFiles()[0]);
      ResourceManager::getInstance()->loadFile(dialog.selectedFiles()[0]);
      refreshResource();
}

void MainWindow::objLoadToggled(QTableWidgetItem* i)
{
      QVariant obj = i->data(Qt::UserRole);
      if(!obj.isNull()) {
            RendererObject* o = static_cast<RendererObject*>(obj.value<void*>());
            bool toggled = i->checkState() == Qt::Checked;
            if(toggled) {
                  RenderWorker::Instance()->addObject(o);
            }
      }
      bool toggled;

}

void MainWindow::refreshResource() {
      //resourceWidget->clear();
      const auto& rec_list = ResourceManager::getInstance()->getResourceList();
      resourceWidget->setRowCount(rec_list.size());
      int idx = 0;
      for (RendererObject* obj : rec_list) {
            auto* item_name = new QTableWidgetItem(obj->name());
            item_name->setData(Qt::UserRole, QVariant::fromValue((void*)obj));
            item_name->setCheckState(Qt::Unchecked);
            auto* item_type = new QTableWidgetItem(obj->type_name());
            resourceWidget->setItem(idx, 0, item_name);
            resourceWidget->setItem(idx, 1, item_type);
            idx++;
      }
}

MainWindow* MainWindow::getInstance() {
      return _mainWindow;
}