#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/shape.h"
class Shapeable;
struct Interaction {
      Point3f pWorld;
      // Primitive* pFrom;
      Shapeable* pTo = nullptr;
      Float u;
      Float v;
      Normal3f n;
      Vector3f dpdu, dpdv;
      Vector3f wo;
      Float t;
      Vector3f GetLocalWo() const {
            Float height = Dot(wo, n);
            Float x = Cross(wo, n).Length();
            return { x, 0, height };
      }
      Vector3f GlobalDirection(Vector3f& localWi) const {
            Vector3f globalXunit = Normalize(Cross(n, Cross(wo, n))); // TODO: make this an operation
            return Vector3f(localWi.z*n) + localWi.y*Normalize(Cross(wo, n)) + localWi.x*globalXunit;

      }
};