#pragma once
#include "core/geometry.h"
#include "core/film.h"
#include "core/scene.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/renderoption.h"
#include "core/tracer.h"
#include <cmath>

class Camera {
      Transform _cam2world; // not used in RTR
      Transform _world2cam;
      // Contrl
      volatile bool _rotationXTrigger = false;
      volatile bool _rotationYTrigger = false;
      Transform _rotation;
      // Config
      Float film_distance = 1.f;
      Film film;
      Float fov_Vertical;
      // Config Used in RTR
      Float clip_near = 0.1f;
      Float clip_far = 1000.f;
      

      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);
public:
      Camera(Scene* s, Transform cam2world, const Point2i& film_size, Float fov_Vertical = 160.f * Pi / 180) : fov_Vertical(fov_Vertical), s(s), tracer(s), _cam2world(cam2world),
            _world2cam(cam2world.Inverse()), film(film_size), film_distance(film_size.y / 2 / std::tan(fov_Vertical / 2)) {}
      void Render(RenderOption& options);
      const Film& getFilm() const { return film; }
      // Generate XMMATRIX using XMMatrixPerspectiveFovLH
      // Generate OpenGL transform, row major order
      Transform Cam2NDC() const;
      const Transform& world2cam() const { return _world2cam; }
      const Transform& cam2world() const { return _cam2world; }
      bool rotationXTrigger() const { return _rotationXTrigger; }
      bool rotationYTrigger() const { return _rotationYTrigger; }
      bool rotationTrigger() const { return _rotationXTrigger || _rotationYTrigger; }
      void setTransform(Float offsetX, Float offsetY);
      void applyRotation();
      
};

Camera* CreateRTCamera();