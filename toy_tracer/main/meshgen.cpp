#include <vector>
#include <functional>
#include "core/shape.h"
#include "core/primitive.h"
#include "core/material.h"
#include "main/ResourceManager.h"
#include "main/MainWindow.h"
#include "shape/rectangular.h"
#include <QString>
#include <QMenu>

static std::vector<Shape*> shapes;
struct ShapeTableEntry {
      QString name;
      std::function<Shape*(void)> action;
      
};

static void fromShapeFunc(std::function<Shape*(void)> shapeFunc) {
      Primitive* p = new Primitive(shapeFunc(), defaultMaterial);
      p->GenMeshes();
      ResourceManager::getInstance()->addPrimitive(p);
      MainWindow::getInstance()->refreshResource();
}
ShapeTableEntry shapeTable[] = {
      {"Sphere", []() {return new Sphere(); }},
      {"Rectangular", []() {return new Rectangular(); }},
      
};

void addCreateShapeMenu(QMenu* shapeMenu) {
      for (auto& e : shapeTable) {
            shapeMenu->addAction(e.name, std::bind(fromShapeFunc, e.action));
      }
      ;
}

// TODO: add and use this whenever we need to create a vertex buffer
void* collectVertexBuffer(std::vector<void*>& pointers, const Layout& l, uint32_t vertex_count) {
      if (l.size() > pointers.size())
            return nullptr;
      unsigned char* ret = new unsigned char[l.strip()*vertex_count];

      return nullptr;
}