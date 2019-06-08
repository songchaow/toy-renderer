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