#include "core/camera.h"
#include <cassert>

Ray Camera::GenerateRay(const Point2f& pFilm)
{
      Vector3f d(pFilm.y - film.getWidth() / 2, film.getHeight() / 2 - pFilm.x, film_distance);
      Ray localRay({ 0.f,0.f,0.f }, Normalize(d));
      return _cam2world(localRay);
}

Transform toNDCPerspective(Float n, Float f, Float aspectRatio, Float fov_Horizontal) {
      Float tanHalfFov_H = std::tan(fov_Horizontal / 2);
      Float tanHalfFov_V = tanHalfFov_H / aspectRatio;
      Matrix4 persp(1.f / tanHalfFov_H, 0, 0, 0,
            0, tanHalfFov_V, 0, 0,
            0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
            0, 0, -1, 0);
      return Transform(persp);
}

const Matrix4& Camera::Cam2NDC() const
{
#if 0
      Matrix4 persp(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, clip_far / (clip_far - clip_near), -clip_far * clip_near / (clip_far - clip_near),
            0, 0, 1, 0);
      // Shift and Scale
      Float height_original = film.getHeight() / film_distance;
      Float width_original = film.getWidth() / film_distance;
      //Transform t = Translate(Vector3f(width_original / 2, height_original / 2, 0));
      Transform t = Translate(Vector3f(0, 0, 0));
      Transform s = Scale(1 / width_original, 1 / height_original, 1);
      return s * t * Transform(persp);
#endif
      //return toNDCPerspective(_near, _far, film.getHeight() / (Float)film.getWidth(), fov_Horizontal);
      if(ndc_transform_dirty) {
            cam2ndc_cache = _view.f.cam2ndc_Perspective();
            ndc_transform_dirty = false;
      }
      return cam2ndc_cache;
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

Matrix4 LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec /*= {0.f, 1.f, 0.f}*/) {
      // First calculate cam2world
      Vector3f localZ = Normalize(-viewDir);
      Vector3f localX = Normalize(Cross(upVec, localZ));
      Vector3f localY = Normalize(Cross(localZ, localX));
      Matrix4 cam2world;
      cam2world[0][0] = localX.x; cam2world[1][0] = localX.y; cam2world[2][0] = localX.z; cam2world[3][0] = 0.f;
      cam2world[0][1] = localY.x; cam2world[1][1] = localY.y; cam2world[2][1] = localY.z; cam2world[3][1] = 0.f;
      cam2world[0][2] = localZ.x; cam2world[1][2] = localZ.y; cam2world[2][2] = localZ.z;
      cam2world[0][3] = pos.x; cam2world[1][3] = pos.y; cam2world[2][3] = pos.z;
      return Inverse(cam2world);
}

void LookAt(const Point3f& pos, const Vector3f& viewDir, const Vector3f& upVec, Matrix4& world2cam) {
      // First calculate cam2world
      Vector3f localZ = Normalize(-viewDir);
      Vector3f localX = Normalize(Cross(upVec, localZ));
      Vector3f localY = Normalize(Cross(localZ, localX));
      world2cam[0][0] = localX.x; world2cam[1][0] = localX.y; world2cam[2][0] = localX.z; world2cam[3][0] = 0.f;
      world2cam[0][1] = localY.x; world2cam[1][1] = localY.y; world2cam[2][1] = localY.z; world2cam[3][1] = 0.f;
      world2cam[0][2] = localZ.x; world2cam[1][2] = localZ.y; world2cam[2][2] = localZ.z;
      world2cam[0][3] = pos.x; world2cam[1][3] = pos.y; world2cam[2][3] = pos.z;

      world2cam = Inverse(world2cam);
}

void Camera::LookAt() {
      // newZ = _viewDir;
      ::LookAt(_pos, _viewDir, Vector3f(0.f, 1.f, 0.f), _view.world2view);
      localX = Normalize(Cross(Vector3f(0.f, 1.f, 0.f), -_viewDir));
#if 0
      localX = Normalize(Cross(_viewDir, Vector3f(0.f, 1.f, 0.f)));
      localY = Normalize(Cross(localX, _viewDir));
      Matrix4& m = _world2cam.m;
      m[0][0] = localX.x; m[1][0] = localX.y; m[2][0] = localX.z; m[3][0] = 0.f;
      m[0][1] = localY.x; m[1][1] = localY.y; m[2][1] = localY.z; m[3][1] = 0.f;
      m[0][2] = _viewDir.x; m[1][2] = _viewDir.y; m[2][2] = _viewDir.z; m[3][2] = 0.f;
      m[0][3] = _pos.x; m[1][3] = _pos.y; m[2][3] = _pos.z; m[3][3] = 1.f;
      _world2cam = Transform(Inverse(m), m);
#endif
#if 0
      // apply spinning
      //Rotate(spinAngle, 0.f);
      //Matrix4 spinM = RotateM(0.f, spinAngle, 0.f) * m;
      Matrix4 spinM = RotateM(0.f, 0, 0.f) * m;
      _world2cam = Transform(Inverse(spinM), spinM);
#endif
      
      if (light_associated) {
            //light.rpos() = spinM(_pos); // apply spinM to camera's pos
            light.rpos() = _pos;
            if(light.isSpotLight())
                  //light.direction() = spinM(_viewDir); // apply spinM to _viewDir
                  light.rdirection() = _viewDir; // apply spinM to _viewDir
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
      Float offset = deltaT * _speed;
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

Matrix4 Frustum::cam2ndc_Perspective() const
{
      Float tanHalfFov_H = std::tan(fov_Horizontal / 2);
      Float tanHalfFov_V = tanHalfFov_H / aspectRatio;
      Matrix4 persp(1.f / tanHalfFov_H, 0, 0, 0,
            0, 1.f / tanHalfFov_V, 0, 0,
            0, 0, -(Far + near) / (Far - near), -2 * Far * near / (Far - near),
            0, 0, -1, 0);
      return persp;
}

Matrix4 Frustum::cam2ndc_Orthogonal() const
{
      // x: [-width/2, width/2] | y: [-height/2, height/2] z:[-1, 1]
      Matrix4 orthogonal(2 / width, 0, 0, 0,
            0, 2 / height, 0, 0,
            0, 0, -2.f/(Far-near), -(Far + near)/(Far-near),
            0, 0, 0, 1);
      return orthogonal;
}
