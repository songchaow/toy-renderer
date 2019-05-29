#include "core/tracer.h"

Spectrum PathTracer::Li(Ray& wo) {
      Spectrum prefix = 1.f;
      Float sum = 0;
      
      // trace from wo
      Interaction i;
      Spectrum Li;
      int currSegment = 0;
      while (currSegment < maxSegment) {
            scene->Intercept(wo, i);
            if (i.pTo->isLight()) {
                  Li = prefix; // + AreaLight::Li
            }
            else {

                  Primitive* p = static_cast<Primitive*>(i.pTo);
                  if (p->getMaterial()->isFlatSurface()) {
                        const FlatMaterial* flat_m = static_cast<const FlatMaterial*>(p->getMaterial());
                        // TODO: reflect?, 
                        Vector3f wi;
                        Spectrum fr = flat_m->delta_f(-wo.d, wi, (Vector3f)i.n, nullptr, true);
                        prefix *= (fr * CosTheta(wi)); // pdf is 1, so not divided
                        wo = Ray(i.pWorld, wi);

                  }
                  else { // GlossMaterial
                        const GlossMaterial* gloss_m = static_cast<const GlossMaterial*>(p->getMaterial());

                  }
            }
      }
      

}