#pragma once

#include "core/material.h"
#include "core/shape.h"

class Primitive
{
private:
    // TODO: general material type should be replaced for SimpleMaterial later.
    //SimpleMaterial* m_material;
      Shape* shape;
      SimpleMaterial* material;
public:
      bool Intercept(const Ray& r, Interaction& i) const { return shape->Intercept(r, i); }
      bool InterceptP(const Ray& r, Interaction* i) const { return shape->InterceptP(r, i); }
      bool ComputeDiff(const Ray& r, Interaction* i) const { return shape->ComputeDiff(r, i); }

};