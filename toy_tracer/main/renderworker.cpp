#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include <QWindow>
#include <QThread>
#include <QDebug>

RenderWorker _worker;

Camera* RenderWorker::cam;

Camera* CreateRTCamera(const Point2i& screen_size) {
      return new Camera(nullptr, Translate(0.f, 0.f, -10.f), screen_size, 40.f * Pi / 180);
}

void RenderWorker::initialize() {
      qDebug() << "RenderWorker Thread:" << QThread::currentThread();
      cam = CreateRTCamera(Point2i(_canvas->width(), _canvas->height()));
      m_context = new QOpenGLContext();
      m_context->setFormat(_canvas->requestedFormat());
      m_context->create();
      m_context->makeCurrent(_canvas);
      initializeOpenGLFunctions();
      GLenum err = glGetError();
      glViewport(0, 0, _canvas->width(), _canvas->height());
      err = glGetError();
      err = glGetError();
}

void RenderWorker::renderLoop() {
      int loopN = 0;
      for(;;) {
            // resize the camera if needed
            assert(_canvas);
            m_context->makeCurrent(_canvas);
            if (_canvas->resized()) {
                  // set glSetViewport
                  // set Camera's size
                  _canvas->clearResized();
            }
            // update the camera's orientation
            if (RenderWorker::cam->rotationTrigger()) {
                  cam->applyRotation();
            }
            if (_canvas->keyPressed())
                  cam->applyTranslation(_canvas->keyStatuses(), 0.01f);
            // add pending primitives
            std::vector<Primitive*> pendingAddPrimitives;
            if (primitiveQueue.readAll(pendingAddPrimitives)) {
                  // loading
                  for (auto* o : pendingAddPrimitives) {
                        // o is primitive now
                        o->load(this);
                        primitives.push_back(o);
                        // if (o->typeID() == RendererObject::TypeID::Primitive) {
                        //       Primitive_Ui* p = static_cast<Primitive_Ui*>(o);
                        //       p->m()->load(this);
                        //       qDebug() << "Primitive " << p << " thread:" << p->thread();
                        //       // TODO: maybe check duplicate in the future
                        //       // time-consuming, but only happens when loading
                        //       primitives.push_back(p->m());
                        // }
                        // else if (o->typeID() == RendererObject::TypeID::Light)
                        //       ;
                  }
            }
            // rendering
            loopN++;
            GLenum err = glGetError();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            err = glGetError();
            //glViewport(0, 0, _canvas->width(), _canvas->height());
            glClear(GL_COLOR_BUFFER_BIT);
            
            for (auto &p : primitives) {
                 if (p->getPBRMaterial()->dirty())
                       p->getPBRMaterial()->update(this);
                 p->draw(this);
            }
            m_context->swapBuffers(_canvas);
      }
}

void RenderWorker::addObject(Primitive* p)
{
      while (!primitiveQueue.addElement(p));
}
