#include "core/scene.h"

bool Scene::Intercept(const Ray& r, Interaction& i) const {
      i.t = 0.f;
      Shapeable* min_s = nullptr;
      for (auto& p : shapes) {
            Interaction tmp;
            if (!p->InterceptP(r, &tmp))
                  continue;
            if (!min_s) {
                  min_s = p;
                  i = tmp;
                  continue;
            }
            if (tmp.t > 0 && tmp.t < i.t) {
                  min_s = p;
                  i = tmp;
            }
      }
      if (!min_s) return false;
      min_s->ComputeDiff(r, &i);
      i.pTo = min_s;
      return true;
}

bool Scene::Visible(Interaction & i, const Vector3f& wi, Light* l) const
{
      Interaction newi;
      Ray shadow_ray(i.pWorld, wi);
      if (!Intercept(shadow_ray, newi))
            // TODO: if point light, return true
            return false;
      return i.pTo == l;
}

Spectrum Scene::SampleDirectLight(Interaction & i) const
{
      for (auto& light : lights) {
            Float pdf;
            Vector3f wi;
            if (light->Sample_wi(sample,i,wi,&pdf)) {
                  if (Visible(i, wi, l)) {
                        ;

                  }
            }
            else
                  // currently sample for the whole surface
                  // TODO: change light::sample_wi to make the return always true
                  return 0.f;
            
                  ;
      }
      return Spectrum();
}
