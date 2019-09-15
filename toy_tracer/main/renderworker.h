#pragma once
#include <QObject>
#include <vector>
#include <mutex>
#include "core/primitive.h"
#include "core/camera.h"
#include "main/TwoThreadQueue.h"
#include "main/canvas.h"

class RenderWorker;
extern RenderWorker _worker;

Camera* CreateRTCamera(const Point2i& screen_size);

class RenderWorker : public QObject, QOpenGLExtraFunctions {
      Q_OBJECT
      QOpenGLContext* m_context;
      TwoThreadQueue<RendererObject*> primitiveQueue;
      std::vector<Primitive*> primitives;
      // TODO: define an update info structure
      static Camera* cam;
      Canvas* _canvas;

public slots:
      void initialize();
      void renderLoop();
public:
      // thread-safe using mutex
      void addObject(RendererObject* p);
      static const Camera* getCamera() { return cam; }
      void setCanvas(Canvas* c) { _canvas = c; }
      static inline RenderWorker* Instance() { return &_worker; }
};

