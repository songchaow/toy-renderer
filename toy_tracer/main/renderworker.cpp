#include "main/renderworker.h"
#include <QWindow>

Camera* RenderWorker::cam;

void RenderWorker::initialize(QWindow* s) {
      m_context = new QOpenGLContext();
      m_context->setFormat(s->requestedFormat());
      m_context->create();
      m_context->makeCurrent(s);
      initializeOpenGLFunctions();
}

void RenderWorker::renderLoop() {
      std::vector<Primitive*> pendingAddPrimitives;
      if (primitiveQueue.readAll(pendingAddPrimitives)) {
            // loading
            for (auto* p : pendingAddPrimitives) {
                  p->load(this);
                  primitives.push_back(p);
            }
      }
      // rendering
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      for (auto &p : primitives) {
            if (p->getPBRMaterial()->dirty())
                  p->getPBRMaterial()->update(this);
            p->draw(this);
      }
}

void RenderWorker::addPrimitive(Primitive* p)
{
      while (!primitiveQueue.addElement(p));
}
