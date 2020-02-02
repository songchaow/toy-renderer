#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include "core/cubemap.h"
#include <glad/glad.h>
#include <QWindow>
#include <QThread>
#include <QDebug>
#include <QOpenGLContext>

RenderWorker _worker;

Camera* RenderWorker::cam = nullptr;

Camera* CreateRTCamera(const Point2i& screen_size) {
      return new Camera(nullptr, Translate(0.f, 0.f, 5.f), screen_size, 90.f * Pi / 180);
}

void RenderWorker::initialize() {
      qDebug() << "RenderWorker Thread:" << QThread::currentThread();
      m_context = new QOpenGLContext();
      QSurfaceFormat format = _canvas->requestedFormat();
      m_context->setFormat(format);
      m_context->create();
      m_context->makeCurrent(_canvas);
      QSurfaceFormat real_fmt = m_context->format();
      int numS = real_fmt.samples();
      if (!gladLoadGL()) {
            LOG(ERROR) << "Load GL functions failed";
            return;
      }
      
      if(!cam)
            cam = CreateRTCamera(Point2i(_canvas->width(), _canvas->height()));
      //initializeOpenGLFunctions();
      GLenum err = glGetError();
      glViewport(0, 0, _canvas->width(), _canvas->height());
      glEnable(GL_DEPTH_TEST);
      //glEnable(GL_CULL_FACE);
      // Associate light to camera
      if (cam->lightAssociated())
            loadPointLight(cam->associatedLight());
      // Skybox
      sky.glLoad();
      TriangleMesh::screenMesh.load();
      // Depth fbo
      glGenFramebuffers(1, &depth_fbo);
      // Multi-sample fbo
      glGenFramebuffers(1, &ms_hdr_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, ms_hdr_fbo);
      glGenTextures(1, &ms_hdr_color);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ms_hdr_color);
      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, NULL);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, _canvas->width(), _canvas->height(), GL_TRUE);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glGenRenderbuffers(1, &ms_hdr_depth);
      
      glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_depth);
      //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());
      // attach them to a new fb
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ms_hdr_color, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ms_hdr_depth);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_MULTISAMPLE);
      GLenum res = glCheckFramebufferStatus(ms_hdr_fbo);
      if (res == GL_FRAMEBUFFER_COMPLETE)
            LOG(INFO) << "complete framebuffer";
      else
            LOG(INFO) << "incomplete";
      // HDR fbo (from ms)
      glGenFramebuffers(1, &hdr_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
      glGenTextures(1, &hdr_color);
      glBindTexture(GL_TEXTURE_2D, hdr_color);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);
      // no depth buffer attached
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      depthMap = new CubeDepthMap();
}

void RenderWorker::renderPassPBR() {
      Shader* shader = LoadShader(PBR, true);
      shader->use();
      // set lights
      uint16_t startPos = shader->getUniformLocation("pointLights[0].pos");
      uint16_t pos = startPos;
      const std::vector<PointLight*>& pointLights = RenderWorker::Instance()->pointLights();
      // 1: vec3 pos
      for (int i = 0; i < pointLights.size(); i++) {
            shader->setUniformF(pos++, pointLights[i]->pos().x, pointLights[i]->pos().y, pointLights[i]->pos().z);
      }
      pos += Shader::maxPointLightNum - pointLights.size();
      // 2: vec3 irradiance
      for (int i = 0; i < pointLights.size(); i++) {
            shader->setUniformF(pos++, pointLights[i]->radiance().rgb[0], pointLights[i]->radiance().rgb[1], pointLights[i]->radiance().rgb[2]);
      }
      pos += Shader::maxPointLightNum - pointLights.size();
      // 3: bool directional
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformBool(pos++, pointLights[i]->isDirectionalLight());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 4: vec3 direction
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(pos++, pointLights[i]->direction());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 5: cosAngle
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(pos++, pointLights[i]->HalfAngle());
      // point shadow
      glActiveTexture(GL_TEXTURE5);
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap->cubeMapObj);
      shader->setUniformI("albedoSampler", 0);
      shader->setUniformI("metallicSampler", 1);
      shader->setUniformI("roughnessSampler", 2);
      shader->setUniformI("emissionSampler", 3);
      shader->setUniformI("aoSampler", 4);
      shader->setUniformI("depthSampler", 5);
      // set camera
      
      shader->setUniformF("world2cam", RenderWorker::getCamera()->world2cam().getRowMajorData());
      shader->setUniformF("cam2ndc", RenderWorker::getCamera()->Cam2NDC().getRowMajorData());
      shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      shader->setUniformF("far", depthMap->depthFarPlane);
      
      //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      for (auto &p : primitives) {
            if (p->getPBRMaterial()->dirty())
                  p->getPBRMaterial()->update();
            p->draw(shader);
      }
}

void RenderWorker::renderPassDepth() {
      Shader* shader = LoadShader(DEPTH_MAP, true);
      //shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      glClearDepth(1.f);
      glClear(GL_DEPTH_BUFFER_BIT);
      for (auto &p : primitives) {
            p->draw(shader);
      }
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
            // add/remove pending primitives
            std::vector<Primitive*> pendingAddPrimitives, pendingDelPrimitives;
            if (primitiveQueue.readAll(pendingAddPrimitives, pendingDelPrimitives)) {
                  for (auto* d : pendingDelPrimitives) {

                  }
                  // loading
                  for (auto* o : pendingAddPrimitives) {
                        // o is primitive now
                        o->load();
                        primitives.push_back(o);
                  }
            }
            // add/remove pending lights
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

            // shadow map
            if (_pointLights.size() > 0) {
                  glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
                  depthMap->o = _pointLights[0]->pos();
                  depthMap->GenCubeDepthMap();
                  //glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
                  glViewport(0, 0, _canvas->width(), _canvas->height());
            }
            // render to ms frame buffer
            loopN++;
            glBindFramebuffer(GL_FRAMEBUFFER, ms_hdr_fbo);
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            sky.draw();
            renderPassPBR();
            // blit to hdr frame buffer
            glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_hdr_fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdr_fbo);
            glBlitFramebuffer(0, 0, _canvas->width(), _canvas->height(), 0, 0, _canvas->width(),
                  _canvas->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
            // render to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            Shader* hdr = LoadShader(HDR_TONE_MAP, true);
            hdr->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, hdr_color);
            hdr->setUniformI("radiance", 0);
            hdr->setUniformF("explosure", 1.0);
            TriangleMesh::screenMesh.glUse();
            glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count()*3, GL_UNSIGNED_INT, nullptr);
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
