#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.x - film.getWidth() / 2, pFilm.y - film.getHeight() / 2, film_distance);
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
      Transform t = Translate(Vector3f(width_original / 2, height_original / 2, 0));
      Transform s = Scale(1 / width_original, 1 / height_original, 1);
      return s * t * Transform(persp);
}

void Camera::Render(RenderOption & options)
{
      // TODO: initialize samplers here
      for (int i = 0; i < film.getWidth(); i++) {
            for (int j = 0; j < film.getHeight(); j++) {
                  // set distribution
                  film.SetSamplePixelRegion(i, j);
                  for (int n = 0; n < options.sample_per_pixel; n++) {
                        Point2f pFilm = film.Sample();
                        // skip faulty points
                        if (pFilm.x == static_cast<Float>(film.getWidth()) || pFilm.y == static_cast<Float>(film.getHeight()))
                        {
                              LOG(WARNING) << "Edge point.";
                              continue;
                        }
                        Ray r = GenerateRay(pFilm);
                        Spectrum li = tracer.Li(r);
                        film.addRay(li, pFilm);
                        ;
                  }
                  
            }
      }
      // Normalize
      film.Normalize();
}
