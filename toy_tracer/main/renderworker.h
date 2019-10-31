#pragma once
#include <QObject>
#include <vector>
#include <mutex>
#include "core/primitive.h"
#include "core/camera.h"
#include "light/point.h"
#include "main/TwoThreadQueue.h"
#include "main/canvas.h"

class RenderWorker;
extern RenderWorker _worker;

Camera* CreateRTCamera(const Point2i& screen_size);

class RenderWorker : public QObject, QOpenGLExtraFunctions {
      Q_OBJECT
      QOpenGLContext* m_context;
      TwoThreadQueue<Primitive*> primitiveQueue;
      TwoThreadQueue<PointLight*> lightQueue;
      std::vector<Primitive*> primitives;
      std::vector<PointLight*> _pointLights;
      // TODO: define an update info structure
      static Camera* cam;
      Canvas* _canvas;

public slots:
      void initialize();
      void renderLoop();
public:
      // thread-safe using mutex
      void loadObject(Primitive* p);
      void loadPointLight(PointLight* l);
      static Camera* getCamera() { return cam; }
      void setCanvas(Canvas* c) { _canvas = c; }
      static inline RenderWorker* Instance() { return &_worker; }
      const std::vector<PointLight*>& pointLights() const { return _pointLights; }
};

