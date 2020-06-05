#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/shape.h"
class Primitive3D;
struct Interaction {
      Point3f pWorld;
      // Primitive* pFrom;
      Primitive3D* pTo = nullptr;
      Float u;
      Float v;
      Normal3f n;
      Vector3f dpdu, dpdv;
      Vector3f wo;
      Vector3f wi; // valid after sampling
      Float t;
      Vector3f GetLocalWo() const {
            Float height = Dot(wo, n);
            Float x = Cross(wo, n).Length();
            return { x, 0, height };
      }
      Vector3f LocalDirection(Vector3f& globalWi) const {
            Float height = Dot(globalWi, n);
            // get globalWi - n
            Vector3f& flatWi = globalWi - Vector3f(n) * height;
            Float x = Dot(flatWi, wo);
            Vector3f checky = Cross(flatWi, wo);
            Float y = Cross(flatWi, wo).Length();
            if (!SameOpposition(n, checky))
                  y = -y;
            return { x,y,height };
      }
      Vector3f GlobalDirection(Vector3f& localWi) const {
            Vector3f globalXunit = Normalize(Cross(n, Cross(wo, n))); // TODO: make this an operation
            return Vector3f(localWi.z*n) + localWi.y*Normalize(Cross(wo, n)) + localWi.x*globalXunit;

      }
};