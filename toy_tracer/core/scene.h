#pragma once
#include <vector>
#include "core/primitive.h"
#include "core/interaction.h"
#include "light/skybox.h"
#include "light/area.h"
class Scene {
      EquiRectSkybox* _skybox = nullptr;
      // including shaped lights and primitives
      std::vector<Shapeable*> shapes;
      std::vector<Light*> lights; // shapeable lights are also included in shapes.
public:
      bool Intercept(const Ray& r, Interaction& i) const;
      // i.wo must be valid
      bool Visible(Interaction & i, const Vector3f& wi, Light* l) const;
      Spectrum SampleDirectLight(Interaction& i) const;
      void setSkybox(Skybox* skybox) { _skybox = skybox; }
      EquiRectSkybox* skybox() const { return _skybox; }
      void AddObj(Shapeable* obj);
};