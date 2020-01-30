#pragma once
#include "core/geometry.h"
#include "core/film.h"
#include "core/scene.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/renderoption.h"
#include "core/tracer.h"
#include "light/point.h"
#include <cmath>

class Camera {
      // Orientation
      Transform _cam2world; // not used in RTR
      Transform _world2cam;
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
      // Config
      Float film_distance = 1.f;
      Film film;
      Float fov_Vertical;
      // Config Used in RTR
      Float _near = 0.1f;
      Float _far = 1000.f;
      // Associated light
      bool light_associated = false;
      PointLight light;
      // Off-line
      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);

public:
      Camera(Scene* s, Transform cam2world, const Point2i& film_size, Float fov_Vertical = 160.f * Pi / 180) : fov_Vertical(fov_Vertical), s(s), tracer(s), _cam2world(cam2world),
            _pos(cam2world.m[0][3], cam2world.m[1][3], cam2world.m[2][3]),
            _world2cam(cam2world.Inverse()), film(film_size), film_distance(film_size.y / 2 / std::tan(fov_Vertical / 2)) {}
      void Render(RenderOption& options);
      const Film& getFilm() const { return film; }
      // Generate XMMATRIX using XMMatrixPerspectiveFovLH
      // Generate OpenGL transform, row major order
      Transform Cam2NDC() const;
      void LookAt();
      // Getters
      const Transform& world2cam() const { return _world2cam; }
      const Transform& cam2world() const { return _cam2world; }
      const Transform& rotation() const { return _rotation; }
      bool rotationXTrigger() const { return _rotationXTrigger; }
      bool rotationYTrigger() const { return _rotationYTrigger; }
      bool rotationTrigger() const { return _rotationXTrigger || _rotationYTrigger; }
      Float nearPlane() const { return _near; }
      Float farPlane() const { return _far; }
      bool lightAssociated() const { return light_associated; }
      PointLight* associatedLight() { return &light; }
      void setAssociatedLight(PointLight l);

      void disociateLight();
      void setOrientationTransform(Float offsetX, Float offsetY);
      void setSpinTransform(Float offset, const Point3f& refPoint);
      void applyRotation();
      void applyTranslation(volatile bool* statuses, Float deltaT);
      Point3f pos() const { return _pos; }
      Point3f& rpos() { return _pos; }
      
};

Camera* CreateRTCamera();