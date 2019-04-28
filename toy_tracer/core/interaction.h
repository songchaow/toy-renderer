#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/primitive.h"
struct Interaction {
      Point3f pWorld;
      Primitive* pFrom;
      Primitive* pTo;
      Float u, v;
      Vector3f dpdu, dpdv;
      Vector3f wo;

};