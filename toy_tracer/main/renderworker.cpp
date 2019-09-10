#include "main/renderworker.h"
#include <QWindow>

RenderWorker _worker;

Camera* RenderWorker::cam;

Camera* CreateRTCamera() {

}

void RenderWorker::initialize(QWindow* s) {
      m_context = new QOpenGLContext();
      m_context->setFormat(s->requestedFormat());
      m_context->create();
      m_context->makeCurrent(s);
      initializeOpenGLFunctions();
}

void RenderWorker::renderLoop() {
      for(;;) {
            // resize the camera if needed

            // update the camera's orientation
            if (RenderWorker::cam->rotationTrigger()) {
                  cam->applyRotation();
            }
            // add pending primitives
            std::vector<RendererObject*> pendingAddPrimitives;
            if (primitiveQueue.readAll(pendingAddPrimitives)) {
                  // loading
                  for (auto* o : pendingAddPrimitives) {
                        if (o->typeID() == RendererObject::TypeID::Primitive) {
                              Primitive* p = static_cast<Primitive*>(o);
                              p->load(this);
                              // TODO: maybe check duplicate in the future
                              // time-consuming, but only happens when loading
                              primitives.push_back(p);
                        }
                        else if (o->typeID() == RendererObject::TypeID::Light)
                              ;
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
}

void RenderWorker::addObject(RendererObject* p)
{
      while (!primitiveQueue.addElement(p));
}
