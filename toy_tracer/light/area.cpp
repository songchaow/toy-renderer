#include "light/area.h"

Spectrum UniformAreaLight::Li(Vector3f &w) const {
      return emit;
}

bool UniformAreaLight::Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const {
      // think: return direction or point on the shape?
      Normal3f n;
      Point3f p = shape->SamplePoint(sample, i, n, pwi);
      wi = Normalize(p - i.pWorld);
      Float lenSquared = (p - i.pWorld).LengthSquared();
      Float cosTheta = Dot(n, wi);
      if (cosTheta > 0)
            return false;
      cosTheta = -cosTheta;
      pwi = pwi * lenSquared / cosTheta;
      return true;
}