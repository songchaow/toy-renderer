#include "light/skybox.h"

Spectrum Skybox::Li(Vector3f& w) const {
      // TODO: use an image map
      return reflection;
}


Vector3f Skybox::Sample_wi(Point2f& sample, Interaction& i, Float* pdf) const {

      if (pdf) *pdf = 1.f / 4 / Pi;
      // TODO: if only transmissive or reflective material is found,
      // only sample hemisphere. Then pdf needs to be changed to 1/2Pi
      return SampleUnitSphere(sample);
}