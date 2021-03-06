#include "core/tracer.h"
#include "light/area.h"

Spectrum PathTracer::Li(Ray& ro) {
      Spectrum prefix = 1.f;
      Float sum = 0;
      
      // trace from ro
      Interaction i;
      Spectrum Li(0.f);
      int currSegment = 0;
      Primitive3D* curr_p = nullptr;
      // TODO: add routtelle in while's condition
      while (currSegment < maxSegment) {
            Vector3f wo;
            if(i.pTo) {
                  /*if (i.pTo->isLight()) {
                        // TODO: general light classes should also have Li method, replace following codes
                        UniformAreaLight* el = static_cast<UniformAreaLight*>(i.pTo);
                        Li = prefix * el->Li(wo);
                        break; // TODO: should be return?
                  }
                  else*/ {
                        Primitive3D* p = static_cast<Primitive3D*>(i.pTo);
                        Material* material = p->getMaterial();
                        Float pdf;
                        Spectrum fr = material->sample_f(i, sampler.Sample2D(), &pdf);
                        prefix *= (fr * AbsDot(i.wi, i.n) / pdf);
                        ro = Ray(i.pWorld, i.wi);
                        // TODO: add self-emitting
                  }

            }
            if (!scene->Intercept(ro, i)) {
                  EquiRectSkybox* sky = scene->skybox();
                  if (sky)
                        Li = prefix * sky->Li(ro.d);
                  else
                        Li = 0.f;
                  return Li;
            }
            wo = -ro.d;
            currSegment++;
      }
      // At last, track to the light
      LOG(INFO) << "Abort tracing";
      if (false) {
            // TODO: self emitting Li(wo)
            // UniformAreaLight* el = static_cast<UniformAreaLight*>(i.pTo);
            // Li = prefix * el->Li(i.wo);
      }
      else {
            Spectrum Ld = scene->SampleDirectLight(i);
            Li = prefix * Ld;
      }

      return Li;
}