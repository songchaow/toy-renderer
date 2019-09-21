#pragma once
#include "core/geometry.h"

#include <QtWidgets/qwidget.h>
#include <QWindow>
#include <QOpenGLExtraFunctions>
#include <QMouseEvent>

class Canvas : public QWindow, public QOpenGLFunctions {
      QOpenGLContext* m_context;
      volatile bool _drag = false;
      volatile bool _resized = false;
      volatile bool _keyPressed = false;
      volatile bool _keyStatuses[4] = { false, false, false, false };
      Point2f _pos0;
public:
      Canvas();

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
      bool keyPressed() const { return _keyPressed; }
      void clearResized() { _resized = false; }
      bool drag() const { return _drag; }
      volatile bool* keyStatuses() { return &_keyStatuses[0]; }
      void clearDrag() { _drag = false; }
protected:
      void mousePressEvent(QMouseEvent *ev) override;
      void mouseReleaseEvent(QMouseEvent *ev) override;
      void mouseMoveEvent(QMouseEvent *ev) override;
      void keyPressEvent(QKeyEvent *ev) override;
      void keyReleaseEvent(QKeyEvent *ev) override;
};