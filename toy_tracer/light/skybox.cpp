#include "light/skybox.h"

Spectrum EquiRectSkybox::Li(Vector3f& w) const {
      Float u = SphericalTheta(w) / Pi;
      Float v = SphericalPhi(w) / 2.f / Pi;
      return _texture->Evaluate(u, v);
}


bool EquiRectSkybox::Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const {

      if (pdf) *pdf = 1.f / 4 / Pi;
      // TODO: if only transmissive or reflective material is found,
      // only sample hemisphere. Then pdf needs to be changed to 1/2Pi
      wi = SampleUnitSphere(sample);
      return true;
}