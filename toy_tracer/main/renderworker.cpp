#include "main/renderworker.h"
#include "main/uiwrapper.h"
#include "main/MainWindow.h"
#include "core/cubemap.h"
#include "core/rendertext.h"
#include "core/profiler.h"
#include "engine/controller.h"
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

void RenderWorker::processLoad() {
      if (loadObjectMutex.try_lock()) {
            // add pending primitives
            glLoadPrimitive();

            // remove ...
            for (auto* d : pendingDelPrimitives) {
                  ;
            }
            pendingDelPrimitives.clear();

            // add/remove pending lights
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

            loadObjectMutex.unlock();
      }
}

void RenderWorker::glLoadPrimitive() {
      for (auto* o : pendingAddPrimitives) {
            // o is primitive now
            o->load();
            if (o->is3D()) {
                  Primitive3D* p3d = static_cast<Primitive3D*>(o);
                  if (!p3d->isInstanced()) {
                        primitives.push_back(p3d);
                  }
                  else {
                        static_cast<InstancedPrimitive*>(p3d)->GenInstancedArray();
                        instancedPrimitives.push_back(static_cast<InstancedPrimitive*>(p3d));
                  }
            }
            else {
                  Primitive2D* p2d = static_cast<Primitive2D*>(o);
                  primitives2D.push_back(p2d);
            }
      }
      pendingAddPrimitives.clear();
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
      glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
      csm.initTexture();
      //glEnable(GL_CULL_FACE);
      // Associate light to camera
      if (cam->lightAssociated())
            loadPointLight(cam->associatedLight());
      // Skybox
      sky.glLoad();
      TriangleMesh::screenMesh.load();
      // LUT
      PBRMaterial::loadLUTFile();
      PBRMaterial::glLoadLUT();
      // Depth fbo
      glGenFramebuffers(1, &depth_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_FRAMEBUFFER_SRGB);
      /*GLenum res = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (res == GL_FRAMEBUFFER_COMPLETE)
            LOG(INFO) << "ms hdr complete framebuffer";
      else
            LOG(INFO) << "ms hdr incomplete";*/

      // HDR fbo, receive rendered results/ main rendering and do post processing
      // 3 color buffers: 2 for blooms
      glGenFramebuffers(1, &hdr_fbo);
      glBindFramebuffer(GL_FRAMEBUFFER, hdr_fbo);
      GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
      //GLenum draw_bufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
      glDrawBuffers(3, draw_bufs);
      glGenTextures(1, &hdr_color);
      glBindTexture(GL_TEXTURE_2D, hdr_color);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _canvas->width(), _canvas->height(), 0, GL_RGBA, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // HDR fbo: motion buffer
      glGenTextures(1, &hdr_motion);
      glBindTexture(GL_TEXTURE_2D, hdr_motion);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _canvas->width(), _canvas->height(), 0, GL_RG, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // object id buffer
      glGenTextures(1, &object_id_buffer);
      glBindTexture(GL_TEXTURE_2D, object_id_buffer);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, _canvas->width(), _canvas->height(), 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
      //err = glGetError();
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      // HDR fbo: depth buffer
      glGenTextures(1, &pp_depth);
      glBindTexture(GL_TEXTURE_2D, pp_depth);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _canvas->width(), _canvas->height(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      /*glGenRenderbuffers(1, &pp_depth);
      glBindRenderbuffer(GL_RENDERBUFFER, pp_depth);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _canvas->width(), _canvas->height());*/
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
      // attachment 1,2 are fixed
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdr_motion, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, object_id_buffer, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pp_depth, 0);
      
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      // char2d prepass fbo
      glGenFramebuffers(1, &char2d_prepass_fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, char2d_prepass_fbo);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pp_depth, 0);

      // char2d fbo
      glGenFramebuffers(1, &char2d_fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, char2d_fbo);
      glDrawBuffers(1, draw_bufs);
      glReadBuffer(GL_NONE);
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0); // CHANGING
      //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, object_id_buffer, 0); // FIXED
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pp_depth, 0);
      // ssbo
      glGenBuffers(1, &occlusion_ssbo);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, occlusion_ssbo);
      // data
      glBufferData(GL_SHADER_STORAGE_BUFFER, 2*sizeof(GLfloat)*4*NUM_2DOBJ*NUM_OBJID, nullptr, GL_DYNAMIC_DRAW);
      glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R16UI, GL_RED_INTEGER, GL_BYTE, nullptr);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, occlusion_ssbo);

      // bloom fbo
      glGenFramebuffers(1, &bloom_fbo);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloom_fbo);


      // Shader uniform initialization
      // PBR
      csm.setCameraView(cam->cameraView());
      for(auto en : {ShaderType::PBR, ShaderType::PBR_FLATTEN}) {
            Shader* pbr = LoadShader(en, true);
            pbr->use();
            uint32_t uniform_loc = pbr->getUniformLocation("zpartition[0]");
            for (int i = 0; i < NUM_CASCADED_SHADOW -1; i++) {
                  pbr->setUniformF(uniform_loc + i, csm.zPartition()[i]);
            }
            punctualLightLocations_pbr.queryLocation(pbr);
            glActiveTexture(GL_TEXTURE6);
            glBindTexture(GL_TEXTURE_2D, PBRMaterial::lut.tbo());
            glActiveTexture(GL_TEXTURE7);
            glBindTexture(GL_TEXTURE_CUBE_MAP, sky.specularMaptbo());
            pbr->setUniformI("albedoSampler", 0);
            pbr->setUniformI("mrSampler", 1);
            pbr->setUniformI("normalSampler", 2);
            pbr->setUniformI("emissionSampler", 3);
            pbr->setUniformI("aoSampler", 4);
            pbr->setUniformI("depthSampler", 5);
            pbr->setUniformI("lut", 6);
            pbr->setUniformI("envSpecular", 7);
      }
      
      // TAA
      Shader* taa = LoadShader(TAA, true);
      taa->use();
      taa->setUniformI("currentColor", 0);
      taa->setUniformI("historyTAAResult", 1);
      taa->setUniformI("motionVector", 2);
      taa->setUniformF("windowSize", _canvas->width(), _canvas->height());
      err = glGetError();
      // CHAR_2D_PREPASS
      Shader* char2d_pre = LoadShader(CHAR_2D_PREPASS, true);
      char2d_pre->use();
      char2d_pre->setUniformI("albedoSampler", 0);
      char2d_pre->setUniformI("objectID", 1);
      char2d_pre->setUniformI("depth3D", 2);

      // CHAR_2D
      Shader* char2d = LoadShader(CHAR_2D, true);
      char2d->use();
      char2d->setUniformI("albedoSampler", 0);
      char2d->setUniformI("objectID", 1);
      profiler.setPhaseNames({ "Depth Map Gen", "PBR Pass", "Downsample(MSAA)", "PostProcess", "Tone Map" });
      // load primitives, including mesh and material
      glLoadPrimitive();
}

void RenderWorker::configPBRShader(Shader* shader) {
      // lights
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
            shader->setUniformF(punctualLightLocations_pbr.irradiance[i], pointLights[i]->radiance().rgb[0], pointLights[i]->radiance().rgb[1], pointLights[i]->radiance().rgb[2]);
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
      pos += Shader::maxPointLightNum - pointLights.size();
      // 7: light size
      for (int i = 0; i < pointLights.size(); i++)
            shader->setUniformF(punctualLightLocations_pbr.size[i], pointLights[i]->lightSize());
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
      Shader* monoShader = LoadShader(PBR_FLATTEN, true);
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
            // update keyboard
            Controller::Instance()->Tick(profiler.durationSecond());
            processLoad();
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
            if (firstFrame || !enableTAA) {
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
                  firstFrame = false;
            }
            else
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);
            err = glGetError();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdr_motion, 0);
            err = glGetError();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0f);
            err = glGetError();
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            err = glGetError();
            //if (drawSkybox)
                  //sky.draw();
            renderPassPBR();
            
            err = glGetError();
            profiler.AddTimeStamp();
            // taa
            unsigned char historyTAAIdx = (currTAAIdx + 1) % numTAABuffer;
            // if first frame, just copy to current taa result buffer
            if (firstFrame || !enableTAA) {
#if 0
                  // another method using glCopyTexSubImage2D
                  err = glGetError();
                  glBindFramebuffer(GL_READ_FRAMEBUFFER, hdr_fbo);
                  glReadBuffer(GL_COLOR_ATTACHMENT0);
                  glBindTexture(GL_TEXTURE_2D, taa_results[currTAAIdx]); // the taa tbo that receives
                  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _canvas->width(), _canvas->height());
                  err = glGetError();
#endif
#if 0
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
                  glActiveTexture(GL_TEXTURE0);
                  glBindTexture(GL_TEXTURE_2D, hdr_color);
                  LoadShader(IMAGE, true)->use();
                  TriangleMesh::screenMesh.glUse();
                  //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
                  glDrawArrays(GL_TRIANGLES, 0, 3);
                  firstFrame = false;
#endif
            }
            else {
                  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, hdr_emissive[0], 0);
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
                  //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
                  // disable depth writing
                  //glDisable(GL_DEPTH_TEST);
                  glDepthMask(GL_FALSE);
                  glDrawArrays(GL_TRIANGLES, 0, 3);
                  glDepthMask(GL_TRUE);
                  //glEnable(GL_DEPTH_TEST);
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
#define RENDER_2DCHAR
#ifdef RENDER_2DCHAR
            // 2D character prepass
            glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, char2d_prepass_fbo);
            //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
            Shader* char2d_prepass = LoadShader(CHAR_2D_PREPASS, true);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, object_id_buffer);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, pp_depth);
            primitives2D[0]->draw(char2d_prepass);
            // barrier
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            // 2D character render
            // write depth, but no depth test
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, char2d_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, taa_results[currTAAIdx], 0);
            Shader* char2d = LoadShader(CHAR_2D, true);
            glActiveTexture(GL_TEXTURE0);

            primitives2D[0]->draw(char2d);
            glEnable(GL_DEPTH_TEST);
            // clear SSBO
            glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R16UI, GL_RED, GL_BYTE, nullptr);
#endif
            // Bloom
            glBindFramebuffer(GL_FRAMEBUFFER, bloom_fbo);
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
            //glClearDepth(1.0f);
            //glClear(GL_DEPTH_BUFFER_BIT);
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
            glDrawArrays(GL_TRIANGLES, 0, 3);
            //glDrawElements(GL_TRIANGLES, TriangleMesh::screenMesh.face_count()*3, GL_UNSIGNED_INT, nullptr);
            glEnable(GL_DEPTH_TEST);
            err = glGetError();
            // Profiling
            profiler.PrintProfile();
            profiler.PrintWorstProfile();
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            m_context->swapBuffers(_canvas);
            err = glGetError();
            profiler.AddTimeStamp();
            
            currTAAIdx = (currTAAIdx + 1) % numTAABuffer;
      }
}

void RenderWorker::loadObject(PrimitiveBase* p)
{
      std::lock_guard<std::mutex> lck(loadObjectMutex);
      pendingAddPrimitives.push_back(p);
      //while (!primitiveQueue.addElement(p));
}

void RenderWorker::loadPointLight(PointLight * l)
{
      std::lock_guard<std::mutex> lck(loadObjectMutex);
      pendingLights.push_back(l);
      //while (!lightQueue.addElement(l));
}

void RenderWorker::removePointLight(PointLight* l) {
      std::lock_guard<std::mutex> lck(loadObjectMutex);
      pendingDelLights.push_back(l);
}
