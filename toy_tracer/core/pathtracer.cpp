#include "core/tracer.h"
#include "light/area.h"

Spectrum PathTracer::Li(Ray& ro) {
      Spectrum prefix = 1.f;
      Float sum = 0;
      
      // trace from ro
      Interaction i;
      Spectrum Li(0.f);
      int currSegment = 0;
      Shapeable* curr_p = nullptr;
      // TODO: add routtelle in while's condition
      while (currSegment < maxSegment) {
            Vector3f wo;
            if(i.pTo) {
                  if (i.pTo->isLight()) {
                        // TODO: general light classes should also have Li method, replace following codes
                        UniformAreaLight* el = static_cast<UniformAreaLight*>(i.pTo);
                        Li = prefix * el->Li(wo);
                        break; // TODO: should be return?
                  }
                  else {
                        Primitive* p = static_cast<Primitive*>(i.pTo);
                        if (p->getMaterial()->isFlatSurface()) {
                              const FlatMaterial* flat_m = static_cast<const FlatMaterial*>(p->getMaterial());
                              Vector3f wi;
                              //fr = flat_m->delta_f(-ro.d, wi, (Vector3f)i.n, nullptr, true);
                              Float pdf;
                              Spectrum fr = flat_m->sample_delta_f(sampler.SampleBool(), wo, wi, (Vector3f)i.n, &pdf);
                              prefix *= (fr * AbsDot(wi, i.n) / pdf);
                              ro = Ray(i.pWorld, wi);

                        }
                        else { // GlossMaterial
                              const GlossMaterial* gloss_m = static_cast<const GlossMaterial*>(p->getMaterial());
                              Vector3f localWi;
                              Float pdf;
                              // wo and wi should be in the correct coordinate!
                              Vector3f localWo = i.GetLocalWo();
                              Spectrum fr = gloss_m->sample_f(sampler.Sample2D(), localWo, localWi, nullptr, &pdf);
                              prefix *= (fr * CosTheta(localWi) / pdf);
                              Vector3f wi = i.GlobalDirection(localWi);
                              ro = Ray(i.pWorld, wi);
                        }
                  }

            }
            scene->Intercept(ro, i);
            wo = -ro.d;
      }
      // At last, track to the light
      if (i.pTo->isLight()) {
            ;
      }


      return Li;

}