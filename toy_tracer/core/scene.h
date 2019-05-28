#pragma once
#include <vector>
#include "core/primitive.h"
#include "core/interaction.h"
class Scene {
      std::vector<Primitive*> primitives;
      bool Intercept(const Ray& r, Interaction& i) const;
};