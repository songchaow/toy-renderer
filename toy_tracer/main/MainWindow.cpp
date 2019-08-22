#include "main/MainWindow.h"
#include "core/ResourceManager.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent/* = Q_NULLPTR*/) {
      setupUi(this);
      resourceWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
      QObject::connect(loadResButton, SIGNAL(clicked()), this, SLOT(loadObj()));
      QObject::connect(resourceWidget, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(showProperties(QTableWidgetItem *)));
}

void MainWindow::showProperties(QTableWidgetItem* obj) {
      RendererObject* robj = static_cast<RendererObject*>(obj->data(Qt::UserRole).value<void*>());
      if (robj)
            robj->addProperties(properties);
}

void MainWindow::loadObj() {
      QFileDialog dialog(this, "Load Resource...", QString(), ResourceManager::getInstance()->filters.join(';'));
      dialog.exec();
      resourcePath->setText(dialog.selectedFiles()[0]);
      ResourceManager::getInstance()->loadFile(dialog.selectedFiles()[0]);
      refreshResource();
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