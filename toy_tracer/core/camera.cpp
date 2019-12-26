#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.y - film.getWidth() / 2, film.getHeight() / 2 - pFilm.x, film_distance);
      Ray localRay({ 0.f,0.f,0.f }, Normalize(d));
      return _cam2world(localRay);
}

Transform Camera::Cam2NDC() const
{
      Matrix4 persp(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, clip_far / (clip_far - clip_near), -clip_far * clip_near / (clip_far - clip_near),
            0, 0, 1, 0);
      // Shift and Scale
      Float height_original = film.getHeight() / film_distance;
      Float width_original = film.getWidth() / film_distance;
      //Transform t = Translate(Vector3f(width_original / 2, height_original / 2, 0));
      Transform t = Translate(Vector3f(0, 0, 0));
      Transform s = Scale(1 / width_original, 1 / height_original, 1);
      return s * t * Transform(persp);
}

void Camera::setOrientationTransform(Float offsetX, Float offsetY) {
      _rotation = Rotate(offsetY/50.f, offsetX/50.f);
      _rotationXTrigger = true;
}

// assume the axis is z for now
// offsest: the angle
void Camera::setSpinTransform(Float offset, const Point3f& refPoint) {
      spinRefPoint = refPoint;
      // accumulate spinAngle
      spinAngle += offset / 50.f;
      while (spinAngle < 0) spinAngle += 2 * Pi;
      while (spinAngle >= 2 * Pi) spinAngle -= 2 * Pi;
}

void Camera::LookAt() {
      // newZ = _viewDir;
      localX = Normalize(Cross(_viewDir, Vector3f(0.f, 1.f, 0.f)));
      localY = Normalize(Cross(localX, _viewDir));
      Matrix4& m = _world2cam.m;
      m[0][0] = localX.x; m[1][0] = localX.y; m[2][0] = localX.z; m[3][0] = 0.f;
      m[0][1] = localY.x; m[1][1] = localY.y; m[2][1] = localY.z; m[3][1] = 0.f;
      m[0][2] = _viewDir.x; m[1][2] = _viewDir.y; m[2][2] = _viewDir.z; m[3][2] = 0.f;
      m[0][3] = _pos.x; m[1][3] = _pos.y; m[2][3] = _pos.z; m[3][3] = 1.f;
      // apply spinning
      //Rotate(spinAngle, 0.f);
      //Matrix4 spinM = RotateM(0.f, spinAngle, 0.f) * m;
      Matrix4 spinM = RotateM(0.f, 0, 0.f) * m;
      _world2cam = Transform(Inverse(spinM), spinM);
      if (light_associated) {
            //light.rpos() = spinM(_pos); // apply spinM to camera's pos
            light.rpos() = _pos;
            if(light.isDirectionalLight())
                  //light.direction() = spinM(_viewDir); // apply spinM to _viewDir
                  light.direction() = _viewDir; // apply spinM to _viewDir
      }
}

void Camera::disociateLight() {
      if (light_associated) {
            light_associated = false;
            // TODO: add the light to delete queue
      }
}

void Camera::setAssociatedLight(PointLight l) { 
      if (!light_associated) {
            light_associated = true; 
            // TODO: add the light to queue
            
      }
      light = l;
}

void Camera::applyTranslation(volatile bool* keyStatuses, Float deltaT) {
      static Float offset = deltaT * _speed;
      if (keyStatuses[0] && keyStatuses[1])
            ; // Skiped
      else if (keyStatuses[0])
            // Front
            _pos += offset * _viewDir;
      else if (keyStatuses[1])
            // Back
            _pos -= offset * _viewDir;
      if (keyStatuses[2] && keyStatuses[3])
            ; // Skipped
      else if (keyStatuses[2])
            // Left
            _pos -= offset * localX;
      else if (keyStatuses[3])
            // Right
            _pos += offset * localX;
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
      for (int i = 0; i < film.getHeight(); i++) {
            for (int j = 0; j < film.getWidth(); j++) {
                  // set distribution
                  film.SetSamplePixelRegion(i, j);
                  for (int n = 0; n < options.sample_per_pixel; n++) {
                        Point2f pFilm = film.Sample();
                        // skip faulty points
                        if (pFilm.x == static_cast<Float>(film.getHeight()) || pFilm.y == static_cast<Float>(film.getWidth()))
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
