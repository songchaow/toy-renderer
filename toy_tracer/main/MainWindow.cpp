#include "main/MainWindow.h"
#include "main/ResourceManager.h"
#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include <QFileDialog>
#include <QMenu>

MainWindow* _mainWindow;

MainWindow::MainWindow(QWidget *parent/* = Q_NULLPTR*/) {
      _mainWindow = this;
      setupUi(this);
      resourceWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      // add menu for the Add button
      QMenu *menu = new QMenu(this);
      QAction* primitiveAdd = menu->addAction("Primitive...");
      QAction* pointLightAdd = menu->addAction("Point Light");
      loadResButton->setMenu(menu);

      QObject::connect(primitiveAdd, SIGNAL(triggered()), this, SLOT(importObj()));
      // TODO: make sure the slot is called directly. Change addPointLight
      QObject::connect(pointLightAdd, SIGNAL(triggered()), this, SLOT(addPointLight()), Qt::DirectConnection);
      QObject::connect(resourceWidget, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem *)), this, SLOT(showProperties(QTableWidgetItem* ,QTableWidgetItem *)));
      QObject::connect(resourceWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(objLoadToggled(QTableWidgetItem *)));
}

void MainWindow::showProperties(QTableWidgetItem* obj, QTableWidgetItem* p) {
      RendererObject* robj = static_cast<RendererObject*>(obj->data(Qt::UserRole).value<void*>());
      if (robj) {
            robj->addProperties(properties);
            if (robj->typeID() == RendererObject::TypeID::Primitive) {
                  //PBRMaterial_Ui new_Ui = PBRMaterial_Ui(static_cast<Primitive_Ui*>(robj)->m()->getPBRMaterial());
                  Primitive_Ui* pUi = static_cast<Primitive_Ui*>(robj);
                  pUi->setMaterialUi(static_cast<Primitive_Ui*>(robj)->m()->getPBRMaterial());
                  if (pUi->isValid() && pUi->materialUi().isValid()) {
                        pUi->materialUi().addProperties(materialWidget);
                  }
            }
            else if (robj->typeID() == RendererObject::TypeID::Light) {
                  PointLight_Ui* lUi = static_cast<PointLight_Ui*>(robj);
                  

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

void MainWindow::addPointLight() {
      PointLight* pl = new PointLight();
      PointLight_Ui* pl_ui = new PointLight_Ui(pl);
      ResourceManager::getInstance()->getResourceList().push_back(pl_ui);
      refreshResource();
      // TODO: Consider directly load the light

}

void MainWindow::objLoadToggled(QTableWidgetItem* i)
{
      QVariant obj = i->data(Qt::UserRole);
      if(!obj.isNull()) {
            RendererObject* o = static_cast<RendererObject*>(obj.value<void*>());
            bool toggled = i->checkState() == Qt::Checked;
            o->setLoad(toggled);
            switch (o->typeID())
            {
            case RendererObject::TypeID::Primitive:
            {
                  Primitive_Ui * pUi = static_cast<Primitive_Ui*>(o);
                  if (toggled) {
                        RenderWorker::Instance()->loadObject(pUi->m());
                  }
                  break;
            }
                  
            case RendererObject::TypeID::Light:
            {
                  PointLight_Ui* lUi = static_cast<PointLight_Ui*>(o);
                  if (toggled) {
                        RenderWorker::Instance()->loadPointLight(lUi->m());
                  }
                  break;
            }
                  
            }
      }
      bool toggled;

}

void MainWindow::refreshResource() {
      //resourceWidget->clear();
      std::vector<RendererObject*> rec_list = ResourceManager::getInstance()->getResourceList();
      resourceWidget->setRowCount(rec_list.size());
      int idx = 0;
      for (const RendererObject* obj : rec_list) {
            auto* item_name = new QTableWidgetItem(obj->name());
            item_name->setData(Qt::UserRole, QVariant::fromValue((void*)obj));
            item_name->setCheckState(obj->isLoad() ? Qt::Checked : Qt::Unchecked);
            auto* item_type = new QTableWidgetItem(obj->type_name());
            resourceWidget->setItem(idx, 0, item_name);
            resourceWidget->setItem(idx, 1, item_type);
            idx++;
      }
}

MainWindow* MainWindow::getInstance() {
      return _mainWindow;
}