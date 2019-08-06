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
      Transform cam2world;
      Transform world2cam;
      Float film_distance = 1.f;
      Film film;
      Float fov_Vertical;
      // Following members are used in RTR
      Float clip_near;
      Float clip_far;
      // gives a transform from world space to NDC
      Transform Cam2NDC();

      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);
public:
      Camera(Scene* s, Transform cam2world, Point2i& film_size, Float fov_Vertical = 60.f * Pi / 180) : fov_Vertical(fov_Vertical), s(s), tracer(s), cam2world(cam2world),
            world2cam(cam2world.Inverse()), film(film_size), film_distance(film_size.y / 2 / std::tan(fov_Vertical / 2)) {}
      void Render(RenderOption& options);
      const Film& getFilm() const { return film; }
      // Generate XMMATRIX using XMMatrixPerspectiveFovLH
      // Generate OpenGL transform
};