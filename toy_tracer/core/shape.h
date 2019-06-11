#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/transform.h"
#include "core/interaction.h"

struct Interaction;
class Shape {
protected:
      Transform world2obj, obj2world;
public:
      Shape(Transform& obj2world) :obj2world(obj2world), world2obj(obj2world.Inverse()) {}
      virtual Float Area() const = 0;
      virtual bool Intercept(const Ray& r, Interaction& i) const = 0;
      // only returns ray's t, and stores useful intermediate results in i
      virtual bool InterceptP(const Ray& r, Interaction* i) const = 0;
      // compute complete Interaction* i after InterceptP
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const = 0;
      virtual Point3f PointfromUV(Float u, Float v, Normal3f* n) const = 0;
      virtual Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const = 0;
};

class Sphere : public Shape {
      Float radius; // radius in object space
public:
      Sphere(Float r, Transform& obj2world) : Shape(obj2world), radius(r) {}
      virtual Float Area() const override { return 4 * Pi*radius*radius; }
      virtual bool Intercept(const Ray& r, Interaction& i) const override;
      virtual bool InterceptP(const Ray& r, Interaction* i) const override;
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const override;
      virtual Point3f PointfromUV(Float u, Float v, Normal3f* n) const override;
      virtual Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const override;

};

Vector3f SampleUnitSphere(const Point2f& sample);

class Shapeable {
public:
      enum ShapeID {
            Primitive,
            Light
      };
protected:
      Shape* shape;
      ShapeID s_id;
public:
      Shapeable(Shape* shape, ShapeID s_id) : shape(shape), s_id(s_id) {}
      Float Area() const { return shape->Area(); }

      bool isPrimitive() const { return s_id == Primitive; }
      bool isLight() const { return s_id == Light; }
      bool Intercept(const Ray& r, Interaction& i) const { return shape->Intercept(r, i); }
      bool InterceptP(const Ray& r, Interaction* i) const { return shape->InterceptP(r, i); }
      bool ComputeDiff(const Ray& r, Interaction* i) const { return shape->ComputeDiff(r, i); }
};