#include <vector>
#include <functional>
#include "core/shape.h"
#include "core/primitive.h"
#include "main/ResourceManager.h"
#include "main/MainWindow.h"
#include <QString>
#include <QMenu>

static std::vector<Shape*> shapes;
struct ShapeTableEntry {
      QString name;
      std::function<Shape*(void)> action;
      
};

static void fromShapeFunc(std::function<Shape*(void)> shapeFunc) {
      Primitive* p = new Primitive(shapeFunc(), nullptr);
      p->GenMeshes();
      ResourceManager::getInstance()->addPrimitive(p);
      MainWindow::getInstance()->refreshResource();
}
ShapeTableEntry shapeTable[] = { {"Sphere", []() {return new Sphere(); }} };

void addCreateShapeMenu(QMenu* shapeMenu) {
      for (auto& e : shapeTable) {
            shapeMenu->addAction(e.name, std::bind(fromShapeFunc, e.action));
      }
      ;
}