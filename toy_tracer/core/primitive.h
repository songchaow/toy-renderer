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
};