#pragma once
#include "core/geometry.h"
#include "core/transform.h"
struct Frustum {
      enum FrustumType {
            Projective, Orthogonal
      };
      FrustumType type;
      Float near;
      Float Far;
      // Orthogonal
      Float width;
      Float height; // not used
      // Prospective
      Float aspectRatio;
      Float fov_Horizontal;
      // default Prospective ctor
      /*Frustum(Float aspectRatio) : near(0.1f), Far(200.f), aspectRatio(aspectRatio),
            fov_Horizontal(90.f * Pi / 180), type(Projective) {}*/
      Frustum(Float aspectRatio) : near(1.f), Far(1500.f), aspectRatio(aspectRatio),
            fov_Horizontal(90.f * Pi / 180), type(Projective) {}
      Frustum() : Frustum(1.6f) {}
      Frustum(Float width, Float height, Float length) : width(width), height(height), near(0.f),
            Far(length), type(Orthogonal) {}
      Matrix4 cam2ndc_Perspective() const;
      Matrix4 cam2ndc_Orthogonal() const;
      static void randomShift_Perspective(Matrix4& cam2ndc);
};

struct View {
      Frustum f;
      Matrix4 world2view;
      Matrix4 world2ndc; // cached
      View() = default;
      View(const Frustum& f, const Matrix4& m) : f(f), world2view(m) {}
};