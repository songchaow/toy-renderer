#include "main/canvas.h"
#include "main/renderworker.h"
#include "main/MainWindow.h"
#include "main/uiwrapper.h"


Canvas::Canvas(int width, int height) {
      QWindow::resize(width, height);
      setSurfaceType(QWindow::OpenGLSurface);
      QSurfaceFormat reqformat;
      //reqformat.setSamples(1);
      reqformat.setMajorVersion(3);
      reqformat.setMinorVersion(3);
      reqformat.setProfile(QSurfaceFormat::CoreProfile);
      reqformat.setSwapInterval(0);
      setFormat(reqformat);
      create();
      QSurfaceFormat actualFormat = format();
      int alphaDepth = actualFormat.alphaBufferSize();
      int blueDepth = actualFormat.blueBufferSize();


}

const Point2f Canvas::lastMouseMove()
{
      static bool firstDrag = false;
      auto pMouse = QCursor::pos();
      Point2f oldPos = _pos0;
      _pos0 = Point2f(pMouse.x(), pMouse.y());
      // if pressed, return the difference
      if (_drag)
            return Point2f(_pos0 - oldPos);
      else
            return Point2f(0,0);
}

void Canvas::mousePressEvent(QMouseEvent *ev) {
      _drag = true;
}

void Canvas::mouseReleaseEvent(QMouseEvent *ev) {
      _drag = false;
}
#if 0
void Canvas::mouseMoveEvent(QMouseEvent *ev) {
      //if (_drag) {
      if(false) {
            // apply transform: cam()->setTransform(t)
            Float dx = ev->pos().x() - _pos0.x, dy = ev->pos().y() - _pos0.y;
            _pendingRotation = Rotate(dx / 50.f, dy / 50.f);
            if (camera_obj) {
                  // TODO: Camera's setTransform's parameters should be of type Transform
                  RenderWorker::getCamera()->setOrientationTransform(dx, dy);
                  RenderWorker::getCamera()->applyRotation();
                  //RenderWorker::getCamera()->setSpinTransform(dx, Point3f(0.f, 0.f, 0.f));
            }
            else {
                  RendererObject* robj = MainWindow::getInstance()->getCurrentItem();
                  if (robj == nullptr)
                        return;
                  if (robj->typeID() == RendererObject::TypeID::Primitive) {
                        static_cast<Primitive_Ui*>(robj)->m()->obj2world().rotate(dx / 50.f, dy / 50.f, 0);
                  }
            }
                  ;
            _pos0 = ev->pos();
      }
}
#endif

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