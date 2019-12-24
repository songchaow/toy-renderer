#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include <QWindow>
#include <QThread>
#include <QDebug>

RenderWorker _worker;

Camera* RenderWorker::cam = nullptr;

Camera* CreateRTCamera(const Point2i& screen_size) {
      return new Camera(nullptr, Translate(0.f, 0.f, -30.f), screen_size, 40.f * Pi / 180);
}

void RenderWorker::initialize() {
      qDebug() << "RenderWorker Thread:" << QThread::currentThread();
      if(!cam)
            cam = CreateRTCamera(Point2i(_canvas->width(), _canvas->height()));
      m_context = new QOpenGLContext();
      m_context->setFormat(_canvas->requestedFormat());
      m_context->create();
      m_context->makeCurrent(_canvas);
      initializeOpenGLFunctions();
      GLenum err = glGetError();
      glViewport(0, 0, _canvas->width(), _canvas->height());
      glEnable(GL_DEPTH_TEST);
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
            else
                  cam->LookAt();
            if (_canvas->keyPressed()) {
                  if (_canvas->CameraorObject())
                        cam->applyTranslation(_canvas->keyStatuses(), 0.01f);
                  else {
                        applyTranslation(curr_primitive->obj2world().translation(), _canvas->keyStatuses(), 0.01f, Vector3f(1.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f));
                        curr_primitive->obj2world().update();
                  }
            }
            // add pending primitives
            std::vector<Primitive*> pendingAddPrimitives, pendingDelPrimitives;
            if (primitiveQueue.readAll(pendingAddPrimitives, pendingDelPrimitives)) {
                  for (auto* d : pendingDelPrimitives) {

                  }
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
            // add pending lights
            std::vector<PointLight*> pendingLights, pendingDelLights;
            if (lightQueue.readAll(pendingLights, pendingDelLights)) {
                  for (auto* d : pendingDelLights) {
                        auto it = std::find(_pointLights.begin(), _pointLights.end(), d);
                        if (it != _pointLights.end()) {
                              _pointLights.erase(it);
                              // TODO: delete primitive if there is one, or add to pendingDelPrimitives
                        }
                  }
                  for (auto* l : pendingLights) {
                        _pointLights.push_back(l);
                        if (l->primitive()) {
                              primitives.push_back(l->primitive());
                        }
                  }
            }
            // rendering
            loopN++;
            GLenum err = glGetError();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            err = glGetError();
            //glViewport(0, 0, _canvas->width(), _canvas->height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            for (auto &p : primitives) {
                 if (p->getPBRMaterial()->dirty())
                       p->getPBRMaterial()->update(this);
                 p->draw(this);
            }
            m_context->swapBuffers(_canvas);
      }
}

void RenderWorker::loadObject(Primitive* p)
{
      while (!primitiveQueue.addElement(p));
}

void RenderWorker::loadPointLight(PointLight * l)
{
      while (!lightQueue.addElement(l));
}

void RenderWorker::removePointLight(PointLight* l) {
      while (!lightQueue.addDeleteElement(l));
}
