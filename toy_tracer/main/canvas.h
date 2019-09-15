#pragma once
#include <QtWidgets/qwidget.h>
#include <QWindow>
#include <QOpenGLExtraFunctions>

class Canvas : public QWindow, public QOpenGLFunctions {
      QOpenGLContext* m_context;
      volatile bool _drag = false;
      volatile bool _resized = false;
public:
      Canvas() { setSurfaceType(QWindow::OpenGLSurface); create(); }

      // deprecated
      void initialize() {
            m_context = new QOpenGLContext();
            m_context->setFormat(requestedFormat());
            m_context->create();
            m_context->makeCurrent(this);
            initializeOpenGLFunctions();
      }
      // deprecated
      void resize() {
            glViewport(0, 0, size().width(), size().height());
      }
      bool resized() const { return _resized; }
      void clearResized() { _resized = false; }
      bool drag() const { return _drag; }
      void clearDrag() { _drag = false; }
protected:
      void mousePressEvent(QMouseEvent *ev) override;
      void mouseReleaseEvent(QMouseEvent *ev) override;
      void mouseMoveEvent(QMouseEvent *ev) override;

};