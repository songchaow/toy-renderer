#include "core/shape.h"
// TODO: move to main
void Shape::addProperties(QWidget* parent) {
      RendererObject::addProperties(parent);
      addConstText("Shape:", QString::fromStdString(shapeName()), parent);
}