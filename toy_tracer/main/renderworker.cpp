#include "main/renderworker.h"
#include <QWindow>

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

}

void RenderWorker::addPrimitive(Primitive* p)
{
      while (!primitiveQueue.addElement(p));
}
