#pragma once
#include <QObject>
#include <vector>
#include <mutex>
#include "core/primitive.h"
#include "core/camera.h"
#include "core/profiler.h"
#include "core/depthmap.h"
#include "light/point.h"
#include "light/skybox.h"
#include "main/TwoThreadQueue.h"
#include "main/canvas.h"

class RenderWorker;
extern RenderWorker _worker;

Camera* CreateRTCamera(const Point2i& screen_size);

class RenderWorker : public QObject {
      Q_OBJECT
      QOpenGLContext* m_context;
      TwoThreadQueue<Primitive*> primitiveQueue;
      TwoThreadQueue<PointLight*> lightQueue;
      std::vector<Primitive*> primitives;
      std::vector<InstancedPrimitive*> instancedPrimitives;
      std::vector<PointLight*> _pointLights;
      std::vector<PointLight*> pendingLights, pendingDelLights;
      std::vector<Primitive*> pendingAddPrimitives, pendingDelPrimitives;
      // TODO: define an update info structure
      static Camera* cam;
      Canvas* _canvas = nullptr;
      Point2i _resolution;
      Profiler profiler;
#if 0
      enum {
            DEPTH_MAP,
            MULTI_SAMPLE_HDR,
            HDR,
            NUM_FBO
      };
      static const int num_fbo = NUM_FBO;
      GLuint fbo[NUM_FBO];
#endif
      // Depth Map
      //CubeDepthMap* depthMap;
      CascadedDepthMap csm;
      bool alreadyClear = false;
      GLuint depth_fbo = 0;
      // HDR Framebuffer
      GLuint ms_hdr_fbo = 0;
      GLuint ms_hdr_color = 0;
      GLuint ms_hdr_emissive = 0;
      GLuint ms_hdr_depth = 0;
      GLuint hdr_fbo;
      GLuint hdr_color;
      GLuint hdr_emissive[2];
      GLuint hdr_depth; // not used now

      Skybox sky;


public:
      Primitive* curr_primitive;
      bool renderPointLight = true;
      bool enableShadowMap = true;
      bool enableBloom = true;
public slots:
      void initialize();
      void renderLoop();
private:
      void configPBRShader(Shader* shader);
      void GenCSM();
public:
      RenderWorker() = default;
      RenderWorker(Camera* c) { cam = c; }
      // thread-safe using mutex
      void loadObject(Primitive* p);
      void loadPointLight(PointLight* l);
      void removePointLight(PointLight* l);
      static Camera* getCamera() { return cam; }
      const Point2i& resolution() const { return _resolution; }
      Skybox& skybox() { return sky; }
      void setCanvas(Canvas* c) { _canvas = c; }
      void setCamera(Camera* c) { cam = c; }
      Canvas* canvas() { return _canvas; }
      static inline RenderWorker* Instance() { return &_worker; }
      const std::vector<PointLight*>& pointLights() const { return _pointLights; }
      void renderPassPBR();
      void renderPassCubeMapDepth();
};

