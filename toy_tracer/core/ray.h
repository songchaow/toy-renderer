#pragma once
#include "core/common.h"
#include "core/geometry.h"
struct Ray {
      Point3f o;
      Vector3f d; // normalized in world space
      Point3f Transmit(const Float t) const { return o + d * t; }
      Ray(Point3f o, Vector3f d) :o(o),d(d) {}
};