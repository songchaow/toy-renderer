#include "light/skybox.h"

Spectrum Skybox::Li(Vector3f& w) const {
      // TODO: use an image map
      Float u = SphericalTheta(w) / Pi;
      Float v = SphericalPhi(w) / 2 / Pi;
      return _texture->Evaluate(u, v);
}


bool Skybox::Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const {

      if (pdf) *pdf = 1.f / 4 / Pi;
      // TODO: if only transmissive or reflective material is found,
      // only sample hemisphere. Then pdf needs to be changed to 1/2Pi
      wi = SampleUnitSphere(sample);
}