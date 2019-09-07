#pragma once
#include <QObject>
#include <vector>
#include <mutex>
#include "core/primitive.h"
#include "core/camera.h"
#include "main/TwoThreadQueue.h"
class RenderWorker : public QObject, QOpenGLExtraFunctions {
      Q_OBJECT
      QOpenGLContext* m_context;
      TwoThreadQueue<Primitive*> primitiveQueue;
      std::vector<Primitive*> primitives;
      // TODO: define an update info structure
      static Camera* cam;

public slots:
      void initialize(QWindow* s);
      void renderLoop();
public:
      // thread-safe using mutex
      void addPrimitive(Primitive* p);
      static const Camera* getCamera() { return cam; }
};