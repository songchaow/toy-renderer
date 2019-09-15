#include "main/canvas.h"

void Canvas::mousePressEvent(QMouseEvent *ev) {
      _drag = true;
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev) {
      _drag = false;
}

void Canvas::mouseMoveEvent(QMouseEvent *ev) {
      if (_drag) {

            // apply transform: cam()->setTransform(t)

      }
}