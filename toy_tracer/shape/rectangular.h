#pragma once
#include "core/shape.h"

class Rectangular : public Shape {
      Float width = 2.f;
      Float height = 2.f;
public:
      std::vector<TriangleMesh*> GenMesh() const;
      Rectangular(Float width, Float height, Transform& r) : width(width), height(height), Shape(r) {}
      Rectangular() : Shape(Transform::Identity()) {}
      virtual std::string shapeName() const { return "Rectangular"; }
      Float Area() const { return width * height; }
      bool Intercept(const Ray& r, Interaction& i) const override;
      bool InterceptP(const Ray& r, Interaction* i) const override;
      // unimplemented
      bool ComputeDiff(const Ray& r, Interaction* i) const override { return false; }
      Point3f PointfromUV(Float u, Float v, Normal3f* n) const override { return Point3f(); }
      Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const override { return Point3f(); }
};