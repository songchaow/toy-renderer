#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include <QWindow>
#include <QThread>
#include <QDebug>

RenderWorker _worker;

Camera* RenderWorker::cam = nullptr;

Camera* CreateRTCamera(const Point2i& screen_size) {
      return new Camera(nullptr, Translate(0.f, 0.f, -5.f), screen_size, 40.f * Pi / 180);
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
      if (cam->lightAssociated())
            loadPointLight(cam->associatedLight());
}

void RenderWorker::renderScene() {
      GLenum err = glGetError();
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      //glViewport(0, 0, _canvas->width(), _canvas->height());
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      for (auto &p : primitives) {
            if (p->getPBRMaterial()->dirty())
                  p->getPBRMaterial()->update(this);
            p->draw(this);
      }
}

void RenderWorker::renderLoop() {
      int loopN = 0;
      // fbo
      GLuint fbo;
      glGenFramebuffers(1, &fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      // depth map
      GLuint depthMap;
      glGenTextures(1, &depthMap);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      constexpr unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
            renderScene();
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
