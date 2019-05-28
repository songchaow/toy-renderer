#include "core/scene.h"

bool Scene::Intercept(const Ray& r, Interaction& i) const {
      i.t = 0.f;
      Interaction tmp;
      Primitive* min_p = nullptr;
      for (auto& p : primitives) {
            if (!p->InterceptP(r, &tmp))
                  continue;
            if (!min_p) {
                  min_p = p;
                  i = tmp;
                  continue;
            }
            if (tmp.t > 0 && tmp.t < i.t) {
                  min_p = p;
                  i = tmp;
            }
      }
      if (!min_p) return false;
      min_p->ComputeDiff(r, &i);
      i.pTo = min_p;
      return true;
}