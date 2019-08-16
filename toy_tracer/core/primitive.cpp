#include "core/primitive.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

Primitive* CreatePrimitiveFromMesh(TriangleMesh* mesh) {
      Primitive* p = new Primitive(nullptr, nullptr);
      p->setMesh(mesh);
      return p;
}

void Primitive::addProperties(QWidget * parent) {
      RendererObject::addProperties(parent);
      QGroupBox* primitive_group = new QGroupBox("Primitive");
      // set current widget's layout
      QVBoxLayout* currentLayout = new QVBoxLayout;
      primitive_group->setLayout(currentLayout);
      // add toolbars for current item
      addConstText("another p:", "this value", primitive_group);
      // add sub toolbars for child items
      QGroupBox* shapeToolBox = new QGroupBox("Shape");
      if(shape)
            shape->addProperties(shapeToolBox);
      currentLayout->addWidget(shapeToolBox);
      parent->layout()->addWidget(primitive_group);
}
