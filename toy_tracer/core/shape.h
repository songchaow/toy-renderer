#pragma once
#include "core/common.h"
#include "core/geometry.h"
#include "core/transform.h"
#include "core/interaction.h"
class Shape {
protected:
      Transform world2obj, obj2world;
public:
      Shape(Transform& obj2world) :obj2world(obj2world), world2obj(obj2world.Inverse()) {}
      virtual bool Intercept(const Ray& r, Interaction& i) const = 0;
      // only returns ray's t, and stores useful intermediate results in i
      virtual bool InterceptP(const Ray& r, Interaction* i) const = 0;
      // compute complete Interaction* i after InterceptP
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const = 0;
};

class Sphere : public Shape {
      Float radius; // radius in object space
public:
      Sphere(Float r, Transform& obj2world) : Shape(obj2world), radius(r) {}
      virtual bool Intercept(const Ray& r, Interaction& i) const override;
      virtual bool InterceptP(const Ray& r, Interaction* i) const override;
      virtual bool ComputeDiff(const Ray& r, Interaction* i) const override;
};