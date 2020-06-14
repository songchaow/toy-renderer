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

class RenderWorker {
      QOpenGLContext* m_context;
      TwoThreadQueue<Primitive3D*> primitiveQueue;
      TwoThreadQueue<PointLight*> lightQueue;
      std::vector<Primitive3D*> primitives;
      std::vector<Primitive2D*> primitives2D;
      std::vector<InstancedPrimitive*> instancedPrimitives;
      std::vector<PointLight*> _pointLights;
      // intermediate storage
      std::vector<PointLight*> pendingLights, pendingDelLights;
      std::vector<PrimitiveBase*> pendingAddPrimitives, pendingDelPrimitives;

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
      GLuint char2d_fbo; // unimplemented
      GLuint char2d_prepass_fbo;
      GLuint object_id_buffer = 0;
      GLuint occlusion_ssbo = 0;

      GLuint hdr_fbo;
      GLuint hdr_color;
      GLuint hdr_motion;
      GLuint taa_fbo; // unimplemented
      GLuint pp_depth;

      GLuint bloom_fbo;
      GLuint hdr_emissive[2];
      
      unsigned char currTAAIdx = 0;
      static const unsigned char numTAABuffer = 2;
      GLuint taa_results[numTAABuffer];
      Skybox sky;
      bool firstFrame = true;


public:
      // attributes
      Primitive3D* curr_primitive;
      bool renderPointLight = true;
      bool enableShadowMap = false;
      bool csm_fixedLightFrustum = true;
      bool csm_stable = true;
      bool enableBloom = false;
      bool drawSkybox = true;
      bool disableClampWhenStatic = true;
      bool enableTAA = false;
public slots:
      void initialize();
      void renderLoop();
private:
      std::mutex loadObjectMutex;
      void configPBRShader(Shader* shader);
      void GenCSM();
      void glLoadPrimitive();
      void processLoad();
public:
      RenderWorker() = default;
      RenderWorker(Camera* c) { cam = c; }
      void start();
      // thread-safe using mutex
      void loadObject(PrimitiveBase* p);
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

