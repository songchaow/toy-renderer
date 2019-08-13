#pragma once
#include <QtWidgets/qwidget.h>
#include <QWindow>
#include <QOpenGLExtraFunctions>

class Canvas : public QWindow, public QOpenGLExtraFunctions {
      QOpenGLContext* m_context;
public:
      Canvas() {
            m_context = new QOpenGLContext();
            m_context->setFormat(requestedFormat());
            m_context->create();
            m_context->makeCurrent(this);
            initializeOpenGLFunctions();
      }

      void resize() {
            glViewport(0, 0, size().width, size().height);
      }

      void render() {
            ;
      }

};