#pragma once

#include "core/material.h"
#include "core/shape.h"

class Primitive : public Shapeable
{
private:
    // TODO: general material type should be replaced for SimpleMaterial later.
    //SimpleMaterial* m_material;
      SimpleMaterial* material;
public:
      Primitive(Shape* shape) :Shapeable(shape, ShapeID::Primitive) {}
      const SimpleMaterial* getMaterial() const { return material; }
};