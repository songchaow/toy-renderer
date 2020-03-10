#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include "core/cubemap.h"
#include "core/rendertext.h"
#include "core/profiler.h"
#include <glad/glad.h>
#include <QWindow>
#include <QThread>
#include <QDebug>
#include <QOpenGLContext>
#include <chrono>

RenderWorker _worker;

Camera* RenderWorker::cam = nullptr;

Camera* CreateRTCamera(const Point2i& screen_size) {
      return new Camera(nullptr, Translate(0.f, 0.f, 5.f), screen_size, 90.f * Pi / 180);
}

void RenderWorker::start() {
      initialize();
      renderLoop();
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
      LOG(INFO) << "GL Version: " << m_context->format().majorVersion() << '.' << m_context->format().minorVersion();
      if(!cam)
            cam = CreateRTCamera(Point2i(_canvas->width(), _canvas->height()));
      //initializeOpenGLFunctions();
      _resolution = Point2i(_canvas->width(), _canvas->height());
      GLenum err = glGetError();
      glViewport(0, 0, _canvas->width(), _canvas->height());
      glEnable(GL_DEPTH_TEST);
      csm.initTexture();
      //glEnable(GL_CULL_FACE);
      // Associate light to camera
      if (cam->lightAssociated())
            loadPointLight(cam->associatedLight());
      // Skybox
      sky.glLoad();
      TriangleMesh::screenMesh.load();
      // Depth fbo
      glGenFramebuffers(1, &depth_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      //// Multi-sample fbo
      //glGenFramebuffers(1, &ms_hdr_fbo);
      //glBindFramebuffer(GL_FRAMEBUFFER, ms_hdr_fbo);
      //glGenRenderbuffers(1, &ms_hdr_color);
      //glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_color);
      //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 1, GL_RGBA16F, _canvas->width(), _canvas->height());
      //glGenRenderbuffers(1, &ms_hdr_emissive);
      //glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_emissive);
      //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 1, GL_RGBA16F, _canvas->width(), _canvas->height());
      //glGenRenderbuffers(1, &ms_hdr_depth);
      //glBindRenderbuffer(GL_RENDERBUFFER, ms_hdr_depth);
      //glRenderbufferStorageMultisample(GL_RENDERBUFFER, 1, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());
      // attach them to a new fb
      /*glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ms_hdr_color);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, ms_hdr_emissive);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ms_hdr_depth);
      
      glDrawBuffers(2, draw_bufs);*/
      glEnable(GL_DEPTH_TEST);
      /*GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (res == GL_FRAMEBUFFER_COMPLETE)
            LOG(INFO) << "ms hdr complete framebuffer";
      else
            LOG(INFO) << "ms hdr incomplete";*/
      // HDR fbo (from ms), receive rendered results/ main rendering and do post processing
      // 3 color buffers: 2 for blooms
      glGenFramebuffers(1, &hdr_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
      GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
      glDrawBuffers(3, draw_bufs);
      glGenTextures(1, &hdr_color);
      glBindTexture(GL_TEXTURE_2D, hdr_color);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // HDR motion buffer
      glGenTextures(1, &hdr_motion);
      glBindTexture(GL_TEXTURE_2D, hdr_motion);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _canvas->width(), _canvas->height(), 0, GL_RG, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // HDR fbo depth buffer
      glGenRenderbuffers(1, &pp_depth);
      glBindRenderbuffer(GL_RENDERBUFFER, pp_depth);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());
      glGenTextures(2, hdr_emissive);
      glGenTextures(2, taa_results);
      for (int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, hdr_emissive[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, taa_results[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }
      // attachment 1 and 2 is fixed
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdr_emissive[0], 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, hdr_motion, 0);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pp_depth);
      
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      // Shader initialization

      csm.setCameraView(cam->cameraView());
      Shader* pbr = LoadShader(PBR, true);
      pbr->use();
      uint32_t uniform_loc = pbr->getUniformLocation("zpartition[0]");
      for (int i = 0; i < NUM_CASCADED_SHADOW -1; i++) {
            pbr->setUniformF(uniform_loc + i, csm.zPartition()[i]);
      }
      Shader* taa = LoadShader(TAA, true);
      taa->use();
      taa->setUniformI("currentColor", 0);
      taa->setUniformI("historyTAAResult", 1);
      taa->setUniformI("motionVector", 2);
      taa->setUniformF("windowSize", _canvas->width(), _canvas->height());
      err = glGetError();
      profiler.setPhaseNames({ "Depth Map Gen", "PBR Pass", "Downsample(MSAA)", "PostProcess", "Tone Map" });
}

void RenderWorker::configPBRShader(Shader* shader) {
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
      // 3: bool spot
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformBool(pos++, pointLights[i]->isSpotLight());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 4: bool directional
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformBool(pos++, pointLights[i]->isDirectionalLight());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 5: vec3 direction
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(pos++, pointLights[i]->direction());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 6: cosAngle
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(pos++, pointLights[i]->HalfAngle());
      pos += Shader::maxPointLightNum - pointLights.size();
      // 7: light size
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(pos++, pointLights[i]->lightSize());
      // point shadow
      glActiveTexture(GL_TEXTURE5);
      glBindTexture(GL_TEXTURE_2D_ARRAY, csm.tbo());
      pos = shader->getUniformLocation("world2lightndc[0]");
      for (int i = 0; i < NUM_CASCADED_SHADOW; i++)
            shader->setUniformF(pos + i, &csm.lightView()[i].world2ndc);
      pos = shader->getUniformLocation("lightfrustumSize[0]");
      for (int i = 0; i < NUM_CASCADED_SHADOW; i++)
            shader->setUniformF(pos + i, csm.lightView()[i].f.width, csm.lightView()[i].f.height,
                  csm.lightView()[i].f.Far);
      // pos = shader->getUniformLocation("world2lightview[0]");
      // for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
      //       shader->setUniformF(pos + i, &csm.lightView()[i].world2view);
      // }
      // textures
      // TODO: move these to `initialize`
      shader->setUniformI("albedoSampler", 0);
      shader->setUniformI("mrSampler", 1);
      shader->setUniformI("normalSampler", 2);
      shader->setUniformI("emissionSampler", 3);
      shader->setUniformI("aoSampler", 4);
      shader->setUniformI("depthSampler", 5);
      // set camera
      const Matrix4& w2c = RenderWorker::getCamera()->world2cam();
      const Matrix4& w2c_prev = RenderWorker::getCamera()->world2camPrev();
      const Matrix4& c2ndc = RenderWorker::getCamera()->Cam2NDC();
      shader->setUniformF("world2cam", &w2c);
      shader->setUniformF("world2camPrev", &w2c_prev);
      shader->setUniformF("cam2ndc", &c2ndc);
      shader->setUniformF("camPos", RenderWorker::getCamera()->pos().x, RenderWorker::getCamera()->pos().y, RenderWorker::getCamera()->pos().z);
      //shader->setUniformF("far", depthMap->depthFarPlane);
}

void RenderWorker::GenCSM()
{
      csm.GenLightViews(_pointLights[0]->direction());
      glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
      glBindTexture(GL_TEXTURE_2D_ARRAY, csm.tbo());
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, csm.tbo(), 0);
      GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      Shader* csmShader = LoadShader(CASCADED_DEPTH_MAP, true);
      csmShader->use();
      csmShader->setUniformBool("directional", _pointLights[0]->isDirectionalLight());
      csmShader->setUniformI("albedoSampler", 0);
      uint32_t u_pos = csmShader->getUniformLocation("world2ndcs[0]");
      for (int i = 0; i < NUM_CASCADED_SHADOW; i++) {
            csm.lightView()[i].world2ndc = csm.lightView()[i].f.cam2ndc_Orthogonal() * csm.lightView()[i].world2view;
            csmShader->setUniformF(u_pos + i, &csm.lightView()[i].world2ndc);
      }
      glClearDepth(1.f);
      glClear(GL_DEPTH_BUFFER_BIT);
      for (auto &p : primitives) {
            p->draw(csmShader);
      }
      glViewport(0, 0, _canvas->width(), _canvas->height());
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

void RenderWorker::renderPassCubeMapDepth() {
      Shader* shader = LoadShader(DEPTH_CUBE_MAP, true);
      Shader* shaderInstance = LoadShader(DEPTH_CUBE_MAP_INSTANCED, true);
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
      for(;;) {
            
            profiler.Clear();
            profiler.AddTimeStamp();
            m_context->makeCurrent(_canvas);
            // resize the camera if needed
            assert(_canvas);
            if (_canvas->resized()) {
                  // set glSetViewport
                  // set Camera's size
                  _canvas->clearResized();
            }
            // update the camera's orientation
            cam->Tick();
            /*if (RenderWorker::cam->rotationTrigger()) {
                  cam->applyRotation();
            }*/
            /*if (_canvas->keyPressed()) {
                  if (!_canvas->CameraorObject()) {
                        applyTranslation(curr_primitive->obj2world().translation(), _canvas->keyStatuses(), 0.01f, Vector3f(1.f, 0.f, 0.f), Vector3f(0.f, 0.f, 1.f));
                        curr_primitive->obj2world().update();
                  }
            }*/
            // add/remove pending primitives
            if (primitiveQueue.readAll(pendingAddPrimitives, pendingDelPrimitives)) {
                  for (auto* d : pendingDelPrimitives) {

                  }
                  // loading
                  for (auto* o : pendingAddPrimitives) {
                        // o is primitive now
                        o->load();
                        if(!o->isInstanced())
                              primitives.push_back(o);
                        else {
                              static_cast<InstancedPrimitive*>(o)->GenInstancedArray();
                              instancedPrimitives.push_back(static_cast<InstancedPrimitive*>(o));
                        }
                  }
                  pendingAddPrimitives.clear();
                  pendingDelPrimitives.clear();
            }
            // add/remove pending lights
            
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
                  pendingLights.clear();
                  pendingDelLights.clear();
            }
            GLuint err = glGetError();
            // shadow map
            if (enableShadowMap && _pointLights.size() > 0) {
                  if (alreadyClear)
                        alreadyClear = false;
                  GenCSM();
            }
            else {
                  if (!alreadyClear) {
                        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
                        // TODO: csm clear

                        glViewport(0, 0, _canvas->width(), _canvas->height());
                        alreadyClear = true;
                  }
            }
            profiler.AddTimeStamp();
            // render to hdr frame buffer
            loopN++;
            err = glGetError();
            glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            err = glGetError();
            if (drawSkybox)
                  sky.draw();
            renderPassPBR();
            profiler.AddTimeStamp();
            // taa
            unsigned char historyTAAIdx = (currTAAIdx + 1) % numTAABuffer;
            // if first frame, just copy to current taa result buffer
            if (firstFrame) {
#if 0
                  // another method using glCopyTexSubImage2D
                  err = glGetError();
                  glBindFramebuffer(GL_READ_FRAMEBUFFER, hdr_fbo);
                  glReadBuffer(GL_COLOR_ATTACHMENT0);
                  glBindTexture(GL_TEXTURE_2D, taa_results[currTAAIdx]); // the taa tbo that receives
                  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _canvas->width(), _canvas->height());
                  err = glGetError();
#endif
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
                  glActiveTexture(GL_TEXTURE0);
                  glBindTexture(GL_TEXTURE_2D, hdr_color);
                  LoadShader(IMAGE, true)->use();
                  TriangleMesh::screenMesh.glUse();
                  glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);
                  firstFrame = false;
            }
            else {
                  // do blending between taa_results[historyTAAIdx] and hdr_color
                  // output to taa_results[currTAAIdx]
                  err = glGetError();
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
                  Shader* taa_shader = LoadShader(TAA, true);
                  taa_shader->use();
                  taa_shader->setUniformBool("disableClampWhenStatic", disableClampWhenStatic);
                  glActiveTexture(GL_TEXTURE0);
                  glBindTexture(GL_TEXTURE_2D, hdr_color);
                  glActiveTexture(GL_TEXTURE1);
                  glBindTexture(GL_TEXTURE_2D, taa_results[historyTAAIdx]);
                  glActiveTexture(GL_TEXTURE2);
                  glBindTexture(GL_TEXTURE_2D, hdr_motion);
                  TriangleMesh::screenMesh.glUse();
                  glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count() * 3, GL_UNSIGNED_INT, nullptr);
            }
            err = glGetError();
            // blit to hdr frame buffer
            // hdr_color, hdr_emissive[0]
#if 0
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
#endif
            profiler.AddTimeStamp();
            // Bloom
            TriangleMesh::screenMesh.glUse(); // now already using
            if (enableBloom) {
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
            }
            err = glGetError();
            profiler.AddTimeStamp();
            // render to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearDepth(1.0f);
            glClear(GL_DEPTH_BUFFER_BIT);
            Shader* hdr = LoadShader(HDR_TONE_MAP, true);
            hdr->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, taa_results[currTAAIdx]);
            //glBindTexture(GL_TEXTURE_2D, hdr_color);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, hdr_emissive[0]);
            err = glGetError();
            hdr->setUniformI("radiance", 0);
            hdr->setUniformI("bloom", 1);
            hdr->setUniformF("explosure", 1.0);
            glDisable(GL_DEPTH_TEST);
            glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count()*3, GL_UNSIGNED_INT, nullptr);
            glEnable(GL_DEPTH_TEST);
            err = glGetError();
            // Profiling
            profiler.PrintProfile();
            profiler.PrintWorstProfile();
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            m_context->swapBuffers(_canvas);
            profiler.AddTimeStamp();
            
            currTAAIdx = (currTAAIdx + 1) % numTAABuffer;
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
