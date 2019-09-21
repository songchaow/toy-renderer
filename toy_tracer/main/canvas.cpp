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

void Canvas::keyPressEvent(QKeyEvent *ev) {
      switch (ev->key()) {
      case Qt::Key_Up:
      case Qt::Key_W:
            _keyStatuses[0] = true;
            break;
      case Qt::Key_Down:
      case Qt::Key_S:
            _keyStatuses[1] = true;
            break;
      case Qt::Key_Left:
      case Qt::Key_A:
            _keyStatuses[2] = true;
            break;
      case Qt::Key_Right:
      case Qt::Key_D:
            _keyStatuses[3] = true;
      }
      _keyPressed = true;
}

void Canvas::keyReleaseEvent(QKeyEvent *ev) {
      switch (ev->key()) {
      case Qt::Key_Up:
      case Qt::Key_W:
            _keyStatuses[0] = false;
            break;
      case Qt::Key_Down:
      case Qt::Key_S:
            _keyStatuses[1] = false;
            break;
      case Qt::Key_Left:
      case Qt::Key_A:
            _keyStatuses[2] = false;
            break;
      case Qt::Key_Right:
      case Qt::Key_D:
            _keyStatuses[3] = false;
      }
      if (!_keyStatuses[0] && !_keyStatuses[1] && !_keyStatuses[2] && !_keyStatuses[3])
            _keyPressed = false;
}