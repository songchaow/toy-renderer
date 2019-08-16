#include "core/shape.h"

void Shape::addProperties(QWidget* parent) {
      RendererObject::addProperties(parent);
      addConstText("Shape:", QString::fromStdString(shapeName()), parent);
}