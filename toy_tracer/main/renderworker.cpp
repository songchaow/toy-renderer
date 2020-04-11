#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include "core/cubemap.h"
#include "core/rendertext.h"
#include "core/cubematrix.h"
#include "stb_image_write.h"
#include <glad/glad.h>
#include <QWindow>
#include <QThread>
#include <QDebug>
#include <QOpenGLContext>
#include <chrono>
#include <fstream>

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
      _resolution = Point2i(_canvas->width(), _canvas->height());
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
      glGenRenderbuffers(1, &ms_hdr_color);
      glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_color);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, _canvas->width(), _canvas->height());
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, _canvas->width(), _canvas->height(), GL_TRUE);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glGenRenderbuffers(1, &ms_hdr_emissive);
      glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_emissive);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA16F, _canvas->width(), _canvas->height());
      glGenRenderbuffers(1, &ms_hdr_depth);
      
      glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_depth);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());
      // attach them to a new fb
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ms_hdr_color);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, ms_hdr_emissive);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ms_hdr_depth);
      GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
      glDrawBuffers(2, draw_bufs);
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
      glGenTextures(2, hdr_emissive);
      for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, hdr_emissive[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      // attachment1 is fixed
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdr_emissive[0], 0);
      // no depth buffer attached
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      depthMap = new CubeDepthMap();
      Shader* pbr = LoadShader(PBR_INSTANCED, true);
      punctualLightLocations_pbr.queryLocation(pbr);
}

void RenderWorker::configPBRShader(Shader* shader) {
      // set lights
      uint16_t startPos = shader->getUniformLocation("pointLights[0].pos");
      uint16_t pos = startPos;
      const std::vector<PointLight*>& pointLights = RenderWorker::Instance()->pointLights();
      // 1: vec3 pos
      for (int i = 0; i < pointLights.size(); i++) {
            shader->setUniformF(punctualLightLocations_pbr.pos[i], pointLights[i]->pos().x, pointLights[i]->pos().y, pointLights[i]->pos().z);
      }
      pos += Shader::maxPointLightNum - pointLights.size();
      // 2: vec3 irradiance
      for (int i = 0; i < pointLights.size(); i++) {
            uint16_t e = glGetError();
            shader->setUniformF(punctualLightLocations_pbr.irradiance[i], pointLights[i]->radiance().rgb[0], pointLights[i]->radiance().rgb[1], pointLights[i]->radiance().rgb[2]);
            e = glGetError();
            e = glGetError();
      }
      pos += Shader::maxPointLightNum - pointLights.size();
      // 3: bool spot
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformBool(punctualLightLocations_pbr.spot[i], pointLights[i]->isSpotLight());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 4: bool directional
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformBool(punctualLightLocations_pbr.directional[i], pointLights[i]->isDirectionalLight());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 5: vec3 direction
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(punctualLightLocations_pbr.direction[i], pointLights[i]->direction());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 6: cosAngle
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(punctualLightLocations_pbr.cosAngle[i], pointLights[i]->HalfAngle());
      // point shadow
      glActiveTexture(GL_TEXTURE5);
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap->cubeMapObj);
      shader->setUniformI("albedoSampler", 0);
      shader->setUniformI("mrSampler", 1);
      shader->setUniformI("normalSampler", 2);
      shader->setUniformI("emissionSampler", 3);
      shader->setUniformI("aoSampler", 4);
      shader->setUniformI("depthSampler", 5);
      // set camera

      shader->setUniformF("world2cam", RenderWorker::getCamera()->world2cam().getRowMajorData());
      shader->setUniformF("cam2ndc", RenderWorker::getCamera()->Cam2NDC().getRowMajorData());
      shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      shader->setUniformF("far", depthMap->depthFarPlane);
}

void RenderWorker::renderPassPBR() {
      Shader* monoShader = LoadShader(PBR, true);
      Shader* instanceShader = LoadShader(PBR_INSTANCED, true);
      if (primitives.size() > 0) {
            monoShader->use();
            configPBRShader(monoShader);
            for (auto &p : primitives) {
                  for (auto& m : p->getPBRMaterial())
                        if (m.dirty())
                              m.update();
                  p->draw(monoShader);
            }
      }
      if (instancedPrimitives.size() > 0) {
            instanceShader->use();
            configPBRShader(instanceShader);
            for (auto &p : instancedPrimitives) {
                  for (auto& m : p->getPBRMaterial())
                        if (m.dirty())
                              m.update();
                  p->draw(instanceShader);
            }
      }
}

void RenderWorker::renderDiff() {
      GLuint err = glGetError();
      GLuint tbos[2];
      err = glGetError();
      glGenTextures(2, tbos);
      err = glGetError();
      for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, tbos[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            err = glGetError();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _resolution.x, _resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            err = glGetError();
            
      }
      err = glGetError();
      renderFrame();
      m_context->swapBuffers(_canvas);
      err = glGetError();
      InstancedPrimitive* matrix = instancedPrimitives[0];
      /*GLuint tmpFbo;
      glGenFramebuffers(1, &tmpFbo);
      err = glGetError();
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, tmpFbo);
      err = glGetError();
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      err = glGetError();
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbos[0], 0);
      err = glGetError();*/
      // copy
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tbos[0]);
      err = glGetError();
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
      err = glGetError();
      glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _resolution.x, _resolution.y);
      err = glGetError();
      // delete the indexed obj
      matrix->remove(differenceIdx);
      matrix->updateInstancedArray();
      // render again
      renderFrame();
      m_context->swapBuffers(_canvas);
      // copy again
      glActiveTexture(GL_TEXTURE1);
      err = glGetError();
      glBindTexture(GL_TEXTURE_2D, tbos[1]);
      err = glGetError();
      glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
      err = glGetError();
      glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _resolution.x, _resolution.y);
      err = glGetError();
      // render the difference
      Shader* diffShader = LoadShader(DIFFERENCE_SHADER, true);
      diffShader->use();
      TriangleMesh::screenMesh.glUse();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, tbos[0]);
      diffShader->setUniformI("firstImg", 0);
      diffShader->setUniformI("secondImg", 1);
      // draw buffer and fbo hasn't changed since last renderFrame
      glClearDepth(1.0f);
      glClear(GL_DEPTH_BUFFER_BIT);
      glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);
      err = glGetError();
      m_context->swapBuffers(_canvas);
      err = glGetError();
      // download
      glReadBuffer(GL_FRONT);
      unsigned char* dataRead = new unsigned char[_resolution.x*_resolution.y * 3];
      glReadPixels(0, 0, _resolution.x, _resolution.y, GL_RGB, GL_UNSIGNED_BYTE, dataRead);
      err = glGetError();
      stbi_flip_vertically_on_write(1);
      stbi_write_png("outputDifference.png", _resolution.x, _resolution.y, 3, dataRead, _resolution.x * 3);
      // download raw output
      std::ofstream fraw("outputRaw.bin");
      fraw.write((char*)dataRead, _resolution.x*_resolution.y * 3);
      delete dataRead;
}

void RenderWorker::renderFrame() {
      // resize the camera if needed
      auto startTime = std::chrono::system_clock::now();
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
                  if (!o->isInstanced())
                        primitives.push_back(o);
                  else {
                        static_cast<InstancedPrimitive*>(o)->GenInstancedArray();
                        instancedPrimitives.push_back(static_cast<InstancedPrimitive*>(o));
                  }
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
            depthMap->l = _pointLights[0];
            depthMap->GenCubeDepthMap();
            //glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
            glViewport(0, 0, _canvas->width(), _canvas->height());
      }
      // render to ms frame buffer
      glBindFramebuffer(GL_FRAMEBUFFER, ms_hdr_fbo);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClearDepth(1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      sky.draw();
      renderPassPBR();
      // blit to hdr frame buffer
      // hdr_color, hdr_emissive[0]
      glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_hdr_fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdr_fbo);
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);
      glReadBuffer(GL_COLOR_ATTACHMENT1);
      glDrawBuffer(GL_COLOR_ATTACHMENT1);
      glBlitFramebuffer(0, 0, _canvas->width(), _canvas->height(), 0, 0, _canvas->width(),
            _canvas->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
      glReadBuffer(GL_COLOR_ATTACHMENT0);
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
      glBlitFramebuffer(0, 0, _canvas->width(), _canvas->height(), 0, 0, _canvas->width(),
            _canvas->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

      // Gaussian blur
      TriangleMesh::screenMesh.glUse();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, hdr_emissive[0]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_emissive[1], 0);
      Shader* blur = LoadShader(GAUSSIAN_BLUR_H, true);
      blur->use();
      blur->setUniformI("color", 0);
      glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);
      glBindTexture(GL_TEXTURE_2D, hdr_emissive[1]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_emissive[0], 0);
      blur = LoadShader(GAUSSIAN_BLUR_V, true);
      blur->use();
      blur->setUniformI("color", 0);
      glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);

      // render to screen
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearDepth(1.0f);
      glClear(GL_DEPTH_BUFFER_BIT);
      Shader* hdr = LoadShader(HDR_TONE_MAP, true);
      hdr->use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, hdr_color);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, hdr_emissive[0]);
      hdr->setUniformI("radiance", 0);
      hdr->setUniformI("bloom", 1);
      hdr->setUniformF("explosure", 1.0);

      glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);
      // Frame rate
      auto endTime = std::chrono::system_clock::now();
      /*std::string text_str("Total frame: ");
      std::chrono::duration<double> elapsed_seconds = endTime - startTime;
      std::string duration_str = std::to_string(elapsed_seconds.count() * 1000);
      text_str += duration_str += " ms";
      renderTextAtTopLeft(text_str, 1.0);*/
      m_context->swapBuffers(_canvas);
}

void RenderWorker::renderPassDepth() {
      Shader* shader = LoadShader(DEPTH_MAP, true);
      Shader* shaderInstance = LoadShader(DEPTH_MAP_INSTANCED, true);
      //shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      glClearDepth(1.f);
      glClear(GL_DEPTH_BUFFER_BIT);
      shader->use();
      for (auto &p : primitives) {
            p->draw(shader);
      }
      shaderInstance->use();
      for (auto &p : instancedPrimitives)
            p->draw(shaderInstance);
}

void RenderWorker::renderLoop() {
      int loopN = 0;
      for (int frameIdx = 0; frameIdx < 2; frameIdx++) {
            renderFrame();
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
