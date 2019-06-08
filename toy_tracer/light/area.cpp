#include "light/area.h"

Spectrum UniformAreaLight::Li(Vector3f &w) const {
      return emit;
}

Vector3f UniformAreaLight::Sample_wi(Point2f& sample, Interaction& i, Float* pArea) const {

}