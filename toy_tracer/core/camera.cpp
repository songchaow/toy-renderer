#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.x, film_distance, pFilm.y);
      Ray localRay({ 0.f,0.f,0.f }, Normalize(d));
      return cam2world(localRay);
}

Transform Camera::Cam2NDC()
{
      Matrix4 persp(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, clip_far / (clip_far - clip_near), -clip_far * clip_near / (clip_far - clip_near),
            0, 0, 1, 0);
      // Scale
      Float height_original = film.getHeight() / film_distance;
      Float width_original = film.getWidth() / film_distance;
      Transform s = Scale(1 / width_original, 1 / height_original, 1);
      Transform t = Translate(Vector3f(-width_original / 2, -height_original / 2, 0));
      return s * t * Transform(persp);
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
      // Normalize
      film.Normalize();
}
