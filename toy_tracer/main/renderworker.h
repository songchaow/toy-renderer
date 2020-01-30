#pragma once
#include <QObject>
#include <vector>
#include <mutex>
#include "core/primitive.h"
#include "core/camera.h"
#include "core/cubemap.h"
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
      std::vector<PointLight*> _pointLights;
      // TODO: define an update info structure
      static Camera* cam;
      Canvas* _canvas = nullptr;
      CubeDepthMap* depthMap;
      Skybox sky;

public:
      Primitive* curr_primitive;

public slots:
      void initialize();
      void renderLoop();
public:
      RenderWorker() = default;
      RenderWorker(Camera* c) { cam = c; }
      // thread-safe using mutex
      void loadObject(Primitive* p);
      void loadPointLight(PointLight* l);
      void removePointLight(PointLight* l);
      static Camera* getCamera() { return cam; }
      Skybox& skybox() { return sky; }
      void setCanvas(Canvas* c) { _canvas = c; }
      void setCamera(Camera* c) { cam = c; }
      Canvas* canvas() { return _canvas; }
      static inline RenderWorker* Instance() { return &_worker; }
      const std::vector<PointLight*>& pointLights() const { return _pointLights; }
      void renderPassPBR();
      void renderPassDepth();
};

