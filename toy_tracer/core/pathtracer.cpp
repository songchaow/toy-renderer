#include "core/tracer.h"

Spectrum PathTracer::Li(Ray& ro) {
      Spectrum prefix = 1.f;
      Float sum = 0;
      
      // trace from ro
      Interaction i;
      Spectrum Li;
      int currSegment = 0;
      while (currSegment < maxSegment) {
            scene->Intercept(ro, i);
            if (i.pTo->isLight()) {
                  Li = prefix; // + AreaLight::Li
            }
            else {
                  Vector3f wo = -ro.d;
                  Primitive* p = static_cast<Primitive*>(i.pTo);
                  if (p->getMaterial()->isFlatSurface()) {
                        const FlatMaterial* flat_m = static_cast<const FlatMaterial*>(p->getMaterial());
                        Vector3f wi;
                               //fr = flat_m->delta_f(-ro.d, wi, (Vector3f)i.n, nullptr, true);
                        Float pdf;
                        Spectrum fr = flat_m->sample_delta_f(sampler.SampleBool(), wo, wi, (Vector3f)i.n, &pdf);
                        prefix *= (fr * CosTheta(wi) / pdf);
                        ro = Ray(i.pWorld, wi);

                  }
                  else { // GlossMaterial
                        const GlossMaterial* gloss_m = static_cast<const GlossMaterial*>(p->getMaterial());
                        Vector3f wi;
                        Float pdf;
                        // wo and wi should be in the correct coordinate!

                        gloss_m->sample_f(sampler.Sample2D(), wo, wi, i.n, nullptr, &pdf);
                  }
            }
      }
      return Li;

}