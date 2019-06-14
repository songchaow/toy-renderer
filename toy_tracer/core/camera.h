#pragma once
#include "core/geometry.h"
#include "core/film.h"
#include "core/scene.h"
#include "core/ray.h"
#include "core/transform.h"
#include "core/renderoption.h"
#include "core/tracer.h"

class Camera {
      Point3f view_point;
      Vector3f d; // normalized
      Transform cam2world; // initialized first
      Transform world2cam;

      Float film_distance;
      Film film;
      Scene* s;
      PathTracer tracer;
      Ray GenerateRay(const Point2f& pFilm);
      void Render(RenderOption& options);
public:
      Camera(Scene* s, Transform cam2world, Point2f& film_size) :s(s),tracer(s), cam2world(cam2world), world2cam(cam2world.Inverse()), film(film_size) {}
};