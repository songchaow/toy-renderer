#pragma once
#include <vector>
#include "core/primitive.h"
#include "core/interaction.h"
#include "light/skybox.h"
class Scene {
      Skybox* skybox = nullptr;
      // including shaped lights and primitives
      std::vector<Shapeable*> shapes;
public:
      bool Intercept(const Ray& r, Interaction& i) const;
};