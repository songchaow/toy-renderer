#include "main/canvas.h"
#include "main/renderworker.h"

Canvas::Canvas() {
      QWindow::resize(800, 800);
      setSurfaceType(QWindow::OpenGLSurface); create();
}

void Canvas::mousePressEvent(QMouseEvent *ev) {
      _drag = true;
      _pos0 = ev->pos();
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev) {
      _drag = false;
}

void Canvas::mouseMoveEvent(QMouseEvent *ev) {
      if (_drag) {
            // apply transform: cam()->setTransform(t)
            RenderWorker::getCamera()->setTransform(ev->pos().x() - _pos0.x, ev->pos().y() - _pos0.y);
            _pos0 = ev->pos();
      }
}