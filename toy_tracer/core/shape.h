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
class TriangleMesh;
class Shape {
protected:
      // TODO: these 2 tranforms are not used
      Transform world2obj, obj2world;
public:
      Shape() = default;
      Shape(Transform& obj2worldIn) :obj2world(obj2worldIn) {
            obj2world.Inverse(&world2obj);
            obj2world.setInverse(&world2obj);
            world2obj.setInverse(&obj2world);
      }
      Shape(Transform&& obj2worldIn) :obj2world(obj2worldIn) {
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
      virtual std::vector<TriangleMesh*> GenMesh() const = 0;
};

class Sphere : public Shape {
      Float radius; // radius in object space

      // GenMesh configs
      unsigned int uSlide = 50;
      unsigned int vSlide = 50;
public:
      Sphere(Float r) : Shape(), radius(r) {}
      Sphere(Float r, Transform& obj2world) : Shape(obj2world), radius(r) {}
      Sphere() : Sphere(1.f, Transform::Identity()) {}
      virtual std::string shapeName() const { return "Sphere"; }
      virtual Float Area() const override { return 4 * Pi*radius*radius; }
      virtual bool Intercept(const Ray& r, Interaction& i) const override;
      virtual bool InterceptP(const Ray& r, Interaction* i) const override;
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const override;
      virtual Point3f PointfromUV(Float u, Float v, Normal3f* n) const override;
      virtual Point3f SamplePoint(Point2f& random, Interaction& i, Normal3f& n, Float* pdf) const override;
      virtual std::vector<TriangleMesh*> GenMesh() const;
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
      AnimatedTransform _obj2world;
public:
      Shapeable(Shape* shape, ShapeID s_id, Transform t) : _shape(shape), s_id(s_id), _obj2world(t) {};
      Shapeable(const std::vector<TriangleMesh*>& meshes) : _meshes(meshes), _shape(nullptr), _obj2world(Transform::Identity()) {}
      Shapeable(Shape* shape, ShapeID s_id) : Shapeable(shape, s_id, Transform::Identity()) {}
      Float Area() const { return _shape->Area(); }
      std::string getShapeName() const { return _shape->shapeName(); }
      bool isPrimitive() const { return s_id == Primitive; }
      bool isLight() const { return s_id == Light; }
      bool Intercept(const Ray& r, Interaction& i) const { return _shape->Intercept(r, i); }
      bool InterceptP(const Ray& r, Interaction* i) const { return _shape->InterceptP(r, i); }
      bool ComputeDiff(const Ray& r, Interaction* i) const { return _shape->ComputeDiff(r, i); }
      std::vector<TriangleMesh*>& getMeshes() { return _meshes; }
      void setMeshes(std::vector<TriangleMesh*> ms) { _meshes = ms; }
      const std::vector<TriangleMesh*>& meshes() { return _meshes; }
      const Shape* shape() const { return _shape; }
      void GenMeshes() { if (_shape) _meshes = _shape->GenMesh(); }
      AnimatedTransform& obj2world() { return _obj2world; }
};