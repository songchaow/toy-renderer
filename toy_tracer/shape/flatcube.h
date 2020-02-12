#pragma once
#include "core/shape.h"
class FlatCube : public Shape {
      static uint16_t _flatCubeIndices[];
      Float cubeLength;
public:
      FlatCube() : cubeLength(1.0f), Shape() {};
      FlatCube(Float cubeLength) : cubeLength(cubeLength), Shape() {}
      virtual std::vector<TriangleMesh*> GenMesh() const override;
      // unimplemented offline rendering functions
      virtual Float Area() const { return 6 * cubeLength*cubeLength; }
      virtual std::string shapeName() const { return "Flat Cube"; }
      virtual bool Intercept(const Ray& r, Interaction& i) const override { return false; }
      virtual bool InterceptP(const Ray& r, Interaction* i) const override { return false; }
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const override { return false; }
      virtual Point3f PointfromUV(Float u, Float v, Normal3f* n) const override { return Point3f(); }
      virtual Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const { return Point3f(); }
};