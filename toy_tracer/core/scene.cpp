#include "core/scene.h"
#include <cassert>

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
      return static_cast<void*>(i.pTo) == static_cast<void*>(l);
}

Spectrum Scene::SampleDirectLight(Interaction & i) const
{
      // material must be gloss
      assert(i.pTo->isPrimitive());
      Primitive* p = static_cast<Primitive*>(i.pTo);
      if (p->getMaterial()->isFlatSurface()) {
            DLOG(WARNING) << "Call SampleDirectLight on a flat surface interaction.";
            return 0;
      }
      assert(p->getMaterial()->isGlossSurface());
      const GlossSurface* material = static_cast<const GlossSurface*>(p->getMaterial());
      Spectrum Ld(0.f);
      for (auto& light : lights) {
            Float pdf;
            Vector3f wi;
            if (light->Sample_wi(sample, i, wi, &pdf)) {
                  if (Visible(i, wi, light)) {
                        Spectrum Li = light->Li(wi);
                        Float cosWi = AbsDot(i.n, wi);
                        Vector3f localWo = i.GetLocalWo();
                        Vector3f localWi = i.LocalDirection(wi);
                        Float fr = material->f(localWo, localWi, Normal3f(0.f, 0.f, 1.f), nullptr);
                        Ld += fr * Li*cosWi / pdf;
                  }
                  else
                        continue;
            }
            else
                  // currently sample for the whole surface
                  // TODO: change light::sample_wi to make the return always true
                  continue;
      }
      return Ld;
}
