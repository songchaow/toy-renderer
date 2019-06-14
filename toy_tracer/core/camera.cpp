#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.x, film_distance, pFilm.y);
      Ray localRay({ 0.f,0.f,0.f }, Normalize(d));
      return cam2world(localRay);
}

void Camera::Render(RenderOption & options)
{
      // TODO: initialize samplers here
      for (int i = 0; i < film.getWidth(); i++) {
            for (int j = 0; j < film.getHeight(); j++) {
                  for (int n = 0; n < options.sample_per_block; n++) {
                        Point2f pFilm = film.SampleFilm(i, j);
                        Ray r = GenerateRay(pFilm);
                        film.addRay(tracer.Li(r), pFilm);
                        ;
                  }
                  
            }
      }
}
