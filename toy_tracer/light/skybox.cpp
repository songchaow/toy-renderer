#include "light/skybox.h"
#include "core/sample.h"

Spectrum Skybox::Li(Ray& r) const {
      // TODO: use an image map
      return reflection;
}


Vector3f Sample_wi(Vector2f& sample, Interaction& i, Float* pdf) {

      if (pdf) *pdf = 1.f / 4 / Pi;
      // TODO: if only transmissive or reflective material is found,
      // only sample hemisphere. Then pdf needs to be changed to 1/2Pi
      return SampleSphere(sample);
}