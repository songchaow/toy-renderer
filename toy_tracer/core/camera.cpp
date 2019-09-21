#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.x - film.getWidth() / 2, pFilm.y - film.getHeight() / 2, film_distance);
      Ray localRay({ 0.f,0.f,0.f }, Normalize(d));
      return _cam2world(localRay);
}

Transform Camera::Cam2NDC() const
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

void Camera::setTransform(Float offsetX, Float offsetY) {
      _rotation = Rotate(offsetY/50.f, offsetX/50.f);
      _rotationXTrigger = true;
}

void Camera::LookAt() {
      // newZ = _viewDir;
      newX = Normalize(Cross(_viewDir, Vector3f(0.f, 1.f, 0.f)));
      newY = Normalize(Cross(newX, _viewDir));
      Matrix4& m = _world2cam.m;
      m[0][0] = newX.x; m[1][0] = newX.y; m[2][0] = newX.z; m[3][0] = 0.f;
      m[0][1] = newY.x; m[1][1] = newY.y; m[2][1] = newY.z; m[3][1] = 0.f;
      m[0][2] = _viewDir.x; m[1][2] = _viewDir.y; m[2][2] = _viewDir.z; m[3][2] = 0.f;
      m[0][3] = _pos.x; m[1][3] = _pos.y; m[2][3] = _pos.z; m[3][3] = 1.f;
      
      _world2cam = Transform(Inverse(m), m);
}

void Camera::applyTranslation(volatile bool* statuses, Float deltaT) {
      static Float offset = deltaT * _speed;
      if (statuses[0] && statuses[1])
            ; // Skiped
      else if (statuses[0])
            // Front
            _pos += offset * _viewDir;
      else if (statuses[1])
            // Back
            _pos -= offset * _viewDir;
      if (statuses[2] && statuses[3])
            ; // Skipped
      else if (statuses[2])
            // Left
            _pos -= offset * newX;
      else if (statuses[3])
            // Right
            _pos += offset * newX;
      LookAt();
}

void Camera::applyRotation() {
      _rotationXTrigger = false;
      _rotationYTrigger = false;
      _viewDir = Normalize(_rotation(_viewDir));
      LookAt();
      //_world2cam = _rotation * _world2cam;
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
