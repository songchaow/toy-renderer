#pragma once
#include "core/geometry.h"
#include "core/film.h"
#include "core/scene.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/renderoption.h"
#include "core/tracer.h"
#include "core/frustum.h"
#include "light/point.h"
#include <cmath>

class Camera {
      // Orientation
      Transform _cam2world; // not used in RTR
      AnimatedTransform _world2cam_a;
      Vector3f _viewDir = { 0.f,0.f,-1.f };
      Point3f _pos = { 0.f, 0.f, -10.f };
      // Contrl
      volatile bool _rotationXTrigger = false;
      volatile bool _rotationYTrigger = false;
      Vector3f localX;
      Vector3f localY;
      Transform _rotation;
      Float _speed = 2.f;
      Float spinAngle = 0.f;
      Point3f spinRefPoint;
      // Frustum
      View _view;
      mutable bool ndc_transform_dirty = true;
      mutable Matrix4 cam2ndc_cache;
      // Associated light
      bool light_associated = false;
      PointLight light;
      // Off-line
      Float film_distance = 1.f;
      Film film;
      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);

public:
      Camera(Scene* s, Transform cam2world, const Point2i& film_size, Float fov_Horizontal = 160.f * Pi / 180) : _view(Frustum((film_size.x / (Float)film_size.y)), cam2world.Inverse().m), s(s), tracer(s), _cam2world(cam2world),
            _pos(cam2world.m[0][3], cam2world.m[1][3], cam2world.m[2][3]), film(film_size), film_distance(film_size.y / 2 / std::tan(fov_Horizontal / 2)) {}
      Camera(const Matrix4& world2cam, Float aspectRatio, Float fov_Horizontal, const Vector3f& viewDir) : _view(Frustum(aspectRatio), world2cam), _pos(Inverse(world2cam)[0][3], Inverse(world2cam)[1][3], Inverse(world2cam)[2][3]), film(Point2i(1,1)), _viewDir(Normalize(viewDir)) {}
      void Render(RenderOption& options);
      const Film& getFilm() const { return film; }
      // Generate XMMATRIX using XMMatrixPerspectiveFovLH
      // Generate OpenGL transform, row major order
      const Matrix4& Cam2NDC() const;
      void LookAt();
      // Getters
      const Matrix4& world2cam() const { return _view.world2view; }
      const View* cameraView() const { return &_view; }
      const Transform& cam2world() const { return _cam2world; }
      const Transform& rotation() const { return _rotation; }
      bool rotationXTrigger() const { return _rotationXTrigger; }
      bool rotationYTrigger() const { return _rotationYTrigger; }
      bool rotationTrigger() const { return _rotationXTrigger || _rotationYTrigger; }
      Float nearPlane() const { return _view.f.near; }
      void setNearFar(Float n, Float f) { _view.f.near = n; _view.f.far = f; ndc_transform_dirty = true; }
      Float farPlane() const { return _view.f.far; }
      bool lightAssociated() const { return light_associated; }
      PointLight* associatedLight() { return &light; }
      void setAssociatedLight(PointLight l);
      Float& rspeed() { return _speed; }
      void disociateLight();
      void setOrientationTransform(Float offsetX, Float offsetY);
      void setSpinTransform(Float offset, const Point3f& refPoint);
      void applyRotation();
      void applyTranslation(volatile bool* statuses, Float deltaT);
      Point3f pos() const { return _pos; }
      Point3f& rpos() { return _pos; }
      
};

Camera* CreateRTCamera();