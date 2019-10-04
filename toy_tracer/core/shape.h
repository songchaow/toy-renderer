#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/transform.h"
#include "core/interaction.h"
#include "main/scene_object.h"

#include <vector>
#include <string>

constexpr Float MIN_DISTANCE = 0.01f;

struct Interaction;
class Shape {
protected:
      Transform world2obj, obj2world;
public:
      Shape(Transform& obj2world) :obj2world(obj2world) {
            obj2world.Inverse(&world2obj);
            obj2world.setInverse(&world2obj);
            world2obj.setInverse(&obj2world);
      }
      virtual Float Area() const = 0;
      virtual std::string shapeName() const = 0;
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
      virtual std::string shapeName() const { return "Sphere"; }
      virtual Float Area() const override { return 4 * Pi*radius*radius; }
      virtual bool Intercept(const Ray& r, Interaction& i) const override;
      virtual bool InterceptP(const Ray& r, Interaction* i) const override;
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const override;
      virtual Point3f PointfromUV(Float u, Float v, Normal3f* n) const override;
      virtual Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const override;

};

Vector3f SampleUnitSphere(const Point2f& sample);

class TriangleMesh;

class Shapeable {
public:
      enum ShapeID {
            Primitive,
            Light
      };
protected:
      Shape* _shape; // nullptr means it's contructed directly by mesh
      ShapeID s_id;
      std::vector<TriangleMesh*> _meshes;
public:
      Shapeable(Shape* shape, ShapeID s_id) : _shape(shape), s_id(s_id) {}
      Float Area() const { return _shape->Area(); }
      std::string getShapeName() const { return _shape->shapeName(); }
      bool isPrimitive() const { return s_id == Primitive; }
      bool isLight() const { return s_id == Light; }
      bool Intercept(const Ray& r, Interaction& i) const { return _shape->Intercept(r, i); }
      bool InterceptP(const Ray& r, Interaction* i) const { return _shape->InterceptP(r, i); }
      bool ComputeDiff(const Ray& r, Interaction* i) const { return _shape->ComputeDiff(r, i); }
      std::vector<TriangleMesh*>& getMeshes() { return _meshes; }
      void setMeshes(std::vector<TriangleMesh*> ms) { _meshes = ms; }
      const Shape* shape() const { return _shape; }
};