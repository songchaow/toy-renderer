#include "main/MainWindow.h"
#include "main/ResourceManager.h"
#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include <QFileDialog>
#include <QMenu>
#include <QScrollArea>

MainWindow* _mainWindow;
void addCreateShapeMenu(QMenu* shapeMenu);

MainWindow::MainWindow(QWidget *parent/* = Q_NULLPTR*/) {
      _mainWindow = this;
      setupUi(this);
      resourceWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      property_container = new QWidget;
      properties->setFrameShape(QFrame::NoFrame);
      properties->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
      properties->setWidgetResizable(true);
      properties->setWidget(property_container);
      // add menu for the Add button
      QMenu *menu = new QMenu(this);
      QAction* primitiveAdd = menu->addAction("Primitive...");
      QAction* pointLightAdd = menu->addAction("Point Light");
      QMenu *shapeMenu = new QMenu("Shapes", this);
      addCreateShapeMenu(shapeMenu);
      menu->addMenu(shapeMenu);
      loadResButton->setMenu(menu);

      QObject::connect(primitiveAdd, SIGNAL(triggered()), this, SLOT(importObj()));
      // TODO: make sure the slot is called directly. Change addPointLight
      QObject::connect(pointLightAdd, SIGNAL(triggered()), this, SLOT(addPointLight()), Qt::DirectConnection);
      QObject::connect(resourceWidget, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem *)), this, SLOT(showProperties(QTableWidgetItem* ,QTableWidgetItem *)));
      QObject::connect(resourceWidget, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(objLoadToggled(QTableWidgetItem *)));
      QObject::connect(controlObj, &QAbstractButton::toggled, this, &MainWindow::viewToggled);
      QObject::connect(controlCam, &QAbstractButton::toggled, this, &MainWindow::viewToggled);
}

void MainWindow::viewToggled(bool checked) {
      if (checked) {
            if (controlCam->isChecked())
                  RenderWorker::Instance()->canvas()->setControlCamera();
            else
                  RenderWorker::Instance()->canvas()->setControlObject();
      }
}

void deleteContentCascaded(QWidget* parent) {
      if (parent->layout()) {
            // Delete all existing widgets, if any.
            if (parent->layout() != NULL)
            {
                  QLayoutItem* item;
                  while ((item = parent->layout()->takeAt(0)) != NULL)
                  {
                        delete item->widget();
                        delete item;
                  }
                  delete parent->layout();
            }
            delete parent->layout();
      }
      QVBoxLayout* globalLayout = new QVBoxLayout;
      parent->setLayout(globalLayout);
}

void MainWindow::showProperties(QTableWidgetItem* obj, QTableWidgetItem* p) {
      RendererObject* robj = static_cast<RendererObject*>(obj->data(Qt::UserRole).value<void*>());
      _currobj = robj;
      deleteContentCascaded(property_container);
      if (robj) {
            robj->addProperties(property_container);
            if (robj->typeID() == RendererObject::TypeID::Primitive) {
                  // add to material panel
                  Primitive_Ui* pUi = static_cast<Primitive_Ui*>(robj);
                  RenderWorker::Instance()->curr_primitive = pUi->m();
                  if (static_cast<Primitive_Ui*>(robj)->m()->getPBRMaterial().size() > 0)
                        pUi->setMaterialUi(&static_cast<Primitive_Ui*>(robj)->m()->getPBRMaterial()[0]);
                  // TODO: list all the materials
                  if (pUi->isValid() && pUi->materialUi().isValid()) {
                        pUi->materialUi().addProperties(materialWidget);
                  }
            }
            else if (robj->typeID() == RendererObject::TypeID::Light) {
                  PointLight_Ui* lUi = static_cast<PointLight_Ui*>(robj);
                  

            }
      }
      else {
            addDefaultProperties(property_container);
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
      addPointLight(new PointLight());
}

void MainWindow::addPointLight(PointLight* pl) {
      PointLight_Ui* pl_ui = new PointLight_Ui(pl);
      ResourceManager::getInstance()->getResourceList().push_back(pl_ui);
      refreshResource();
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
                  else {

                  }
                  break;
            }
                  
            case RendererObject::TypeID::Light:
            {
                  PointLight_Ui* lUi = static_cast<PointLight_Ui*>(o);
                  if (toggled) {
                        RenderWorker::Instance()->loadPointLight(lUi->m());
                  }
                  else {
                        RenderWorker::Instance()->removePointLight(lUi->m());
                  }
                  break;
            }
                  
            }
      }
      bool toggled;

}

void MainWindow::refreshResource() {
      //resourceWidget->clear();
      resourceWidget->blockSignals(true);
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
      resourceWidget->blockSignals(false);
}

MainWindow* MainWindow::getInstance() {
      return _mainWindow;
}