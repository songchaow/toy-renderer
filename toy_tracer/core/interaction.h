#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/primitive.h"
struct Interaction {
      Point3f pWorld;
      // Primitive* pFrom;
      Shapeable* pTo;
      Float u, v;
      Normal3f n;
      Vector3f dpdu, dpdv;
      Vector3f wo;
      Float t;
};