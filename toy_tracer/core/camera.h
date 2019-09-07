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
      Transform _cam2world;
      Transform _world2cam;
      Float film_distance = 1.f;
      Film film;
      Float fov_Vertical;
      // Following members are used in RTR
      Float clip_near;
      Float clip_far;
      

      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);
public:
      Camera(Scene* s, Transform cam2world, Point2i& film_size, Float fov_Vertical = 160.f * Pi / 180) : fov_Vertical(fov_Vertical), s(s), tracer(s), _cam2world(cam2world),
            _world2cam(cam2world.Inverse()), film(film_size), film_distance(film_size.y / 2 / std::tan(fov_Vertical / 2)) {}
      void Render(RenderOption& options);
      const Film& getFilm() const { return film; }
      // Generate XMMATRIX using XMMatrixPerspectiveFovLH
      // Generate OpenGL transform, row major order
      Transform Cam2NDC() const;
      const Transform& world2cam() const { return _world2cam; }
      const Transform& cam2world() const { return _cam2world; }
};