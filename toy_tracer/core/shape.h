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
      virtual bool Intercept(const Ray& r, Interaction& i) = 0;
};

class Sphere : public Shape {
      Float r; // radius in object space
public:
      Sphere(Float r, Transform& obj2world) : Shape(obj2world), r(r) {}
      virtual bool Intercept(const Ray& r, Interaction& i) override;
};