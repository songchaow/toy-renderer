#include "main/uiwrapper.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QDebug>

void PBRMaterial_Ui::updateProperties() {
      if (!albedo_text->text().isEmpty())
            _m->albedo_map().resetImage(Image::CreateImageFromFile(albedo_text->text().toStdString()));
      if (!metallic_text->text().isEmpty())
            _m->metallic_map().resetImage(Image::CreateImageFromFile(metallic_text->text().toStdString()));
      if (!rough_text->text().isEmpty())
            _m->rough_map().resetImage(Image::CreateImageFromFile(rough_text->text().toStdString()));
      _m->setDirty();
}

void PBRMaterial_Ui::addProperties(QWidget* parent) const {
      //RendererObject::addProperties(parent);
      // TODO: add existing values
      qDebug() << "PBRMaterial thread affinity:" << thread();
      QVBoxLayout* globalLayout = new QVBoxLayout;
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
      }
      parent->setLayout(globalLayout);
      albedo_text = RendererObject::addFileDialog("Albedo Map:", "Open", parent, QString::fromStdString(_m->albedo_map().path()));
      metallic_text = RendererObject::addFileDialog("Metallic Map:", "Open:", parent, QString::fromStdString(_m->metallic_map().path()));
      rough_text = RendererObject::addFileDialog("Rough Map:", "Open:", parent, QString::fromStdString(_m->rough_map().path()));
      // add create menu
      auto* parentLayout = parent->layout();
      QPushButton* update = new QPushButton("Update Material");
      parentLayout->addWidget(update);
      QObject::connect(update, &QPushButton::clicked, this, &PBRMaterial_Ui::updateProperties);
}

void Primitive_Ui::addProperties(QWidget* parent) const {
      RendererObject::addProperties(parent);
      QGroupBox* primitive_group = new QGroupBox("Primitive");
      // set current widget's layout
      QVBoxLayout* currentLayout = new QVBoxLayout;
      primitive_group->setLayout(currentLayout);
      // add toolbars for current item
      addConstText("another p:", "this value", primitive_group);
      // add sub toolbars for child items
      QGroupBox* shapeToolBox = new QGroupBox("Shape");
      shape.reset(_m->shape());
      if(shape.isValid()) {
            // TODO: use Shape_Ui and assign to the 
            shape.addProperties(shapeToolBox);
      }
      currentLayout->addWidget(shapeToolBox);
      parent->layout()->addWidget(primitive_group);
}

void Shape_Ui::addProperties(QWidget* parent) const {
      RendererObject::addProperties(parent);
      addConstText("Shape:", QString::fromStdString(_m->shapeName()), parent);
}