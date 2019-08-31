#include "core/material.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include <cassert>
#include <algorithm>
#include "core/common.h"
#include "core/sampler.h"

Spectrum Material::sample_f(Interaction& i, Point2f sample, Float* pdf)
{
      int index = childSampler.sample(_sampler.Sample1D());
      Material* chosen = children[index];
      Spectrum ret = chosen->sample_f(i, sample, pdf);
      *pdf *= probabilities[index];
      return ret;
}

Spectrum SimpleMaterial::sample_f(Interaction& i, Point2f sample, Float* pdf)
{
      Spectrum throughput;
      if(surface->isFlatSurface()) {
            FlatSurface* flat = static_cast<FlatSurface*>(surface);
            bool reflect = sample[0] > 0.5;
            throughput = flat->sample_delta_f(reflect, i.wo, i.wi, i.n, pdf);
      }
      else {
            GlossSurface* gloss = static_cast<GlossSurface*>(surface);
            // wo must be transformed
            throughput = gloss->sample_f(sample, i.GetLocalWo(), i.wi, nullptr, pdf);
            i.wi = i.GlobalDirection(i.wi);
      }
      Spectrum color = albedo_texture->Evaluate(i.u, i.v);
      return throughput * color;
}

Spectrum SimpleMaterial::f(const Interaction& i)
{
      if(surface->isFlatSurface())
            return 0.f;
      Spectrum color = albedo_texture->Evaluate(i.u, i.v);
      GlossSurface* gloss = static_cast<GlossSurface*>(surface);
      // needn't be in local space
      Spectrum fr = gloss->f(i.wo, i.wi, i.n, nullptr);
      return color * fr;
}

Spectrum Material::f(const Interaction& i)
{
      Spectrum sum;
      for(auto m : children) {
            if(m->isFlat())
                  continue;
            sum += m->f(i);
      }
      return sum;
}

bool Material::isFlat()
{
      if(isflat_cache)
            return *isflat_cache;
      for(auto m : children) {
            if(!m->isFlat()) {
                  isflat_cache.reset(new bool(false));
                  return false;
            }
      }
      isflat_cache.reset(new bool(true));
      return true;
}

void Material::AddSubMaterial(Material * m, Float scale)
{
      if (children.empty()) {
            probabilities.push_back(1.f);
            children.push_back(m);
            return;
      }
      Float shrinkRatio = 1.f;
      if (scale > 0.f)
            shrinkRatio = 1 - scale;
      else {
            shrinkRatio = children.size() / (children.size() + 1);
            scale = 1 / (children.size() + 1);
      }
      // shrink old children
      for (auto& p : probabilities)
            p *= shrinkRatio;
      probabilities.push_back(scale);
      children.push_back(m);
      // update sampler
      childSampler = StepSampler(probabilities);
}

Dielectric vacuum(1.f);

Spectrum Dielectric::Fr(Float cosWi, const ObjectMedium* out_medium, Float cosWt/* = 0*/) const
{
      Float eta_i;
      if (out_medium == nullptr)
            eta_i = 1.f;
      if (out_medium && out_medium->m_type == ObjectMedium::Dielectric)
            eta_i = static_cast<const Dielectric*>(out_medium)->eta;
      // Light cannot transmit out of metal
      if (out_medium && out_medium->m_type == ObjectMedium::Metal) {
            LOG(WARNING) << "Light cannot transmit out of metal";
            return 0;
      }
      if (cosWt == 0) {
            Float sinWi = std::sqrt(1 - cosWi * cosWi);
            Float sinWt = sinWi * eta / eta_i;
            if (sinWt > 1) return 1;
                  cosWt = std::sqrt(1 - sinWt * sinWt);
      }
      Float rs = (eta*cosWi - eta_i * cosWt) / (eta*cosWi + eta_i * cosWt);
      Float rp = (eta_i*cosWi - eta * cosWt) / (eta_i*cosWi + eta * cosWt);
      return (rs * rs + rp * rp) / 2;
}





Spectrum FlatSurface::delta_f(const Vector3f& wo, Vector3f & wi, Normal3f n, Surface* out_material, bool reflect) const
{
      bool revert = SameOpposition(Normal3f(n), wo) ^ reflect;
    if (out_material && out_material->m_surface == Surface::Gloss) {
        LOG(WARNING) << "Transmissing from gloss material";
        return 0.f;
    }
    
    // Calculate medium, eta_in and eta_out
    const Dielectric* out_medium = nullptr;
      Float eta_in, eta_out;
      if (out_material) {
            // gloss material shouldn't be processed in this function
            FlatSurface* flat_out = static_cast<FlatSurface*>(out_material);
            // out_material can't be other materials
            if (flat_out->medium->m_type == ObjectMedium::Metal) {
                  LOG(WARNING) << "Tranmissing from metal to electric.";
                  return 0.f;
            }
            out_medium = static_cast<const Dielectric*>(flat_out->medium);
            eta_out = out_medium->GetEta();
      }
      else
            eta_out = 1.f;
      const Dielectric* din = static_cast<const Dielectric*>(medium);
      eta_in = din->GetEta();
      if (revert) {
            std::swap(eta_in, eta_out);
            n = -n;
      }
    if (reflect)
    {
        // wo known; wi=wo; calculate wt;
        wi = Reflect(wo, n);
        Vector3f wt;
        Spectrum fr;
        Float cosThetaI = Dot(wi, n);
        DLOG_IF(ERROR, cosThetaI < 0) << "cosThetaI < 0";
        if (!Refract(wi, n, eta_in / eta_out, wt))
              fr = 1.f;
        else {
              // wt is in the other half sphere, so revert the returned value
              Float cosThetaT = -Dot(wt, n);
              DLOG_IF(ERROR, cosThetaT < 0) << "cosThetaT < 0";
              fr = medium->Fr(cosThetaI, out_medium, cosThetaT);
        }
        
        return fr / cosThetaI;
    }
    else
    {
        // wt known; wo=wt; calculate wi;
        if (!Refract(wo, -n, eta_in / eta_out, wi)) {
              DLOG(WARNING) << "Total Reflection when refracting";
              return 0;
        }
        Float cosThetaI = Dot(wi, n); // dot itself is positive
        DLOG_IF(ERROR, cosThetaI < 0) << "cosThetaI < 0";
        Float cosWo = -Dot(n, wo); // dot itself is negative
        DLOG_IF(ERROR, cosWo < 0) << "cosWo < 0";
        auto fr = medium->Fr(cosThetaI, out_medium, cosWo);
        return (- fr + 1.f) / cosThetaI * eta_in*eta_in / eta_out / eta_out;
    }
}

Spectrum FlatSurface::sample_delta_f(bool sample, const Vector3f& wo, Vector3f & wi, Normal3f n, Float* pdf) const {
      // currently randomly choose from reflection and transmission
      // TODO: sample according to r_mask and t_mask
      if (medium->isDielectric() && !Refract(wo, -n, 1.f / static_cast<const Dielectric*>(medium)->GetEta(), wi))
            sample = true;
      if (pdf) *pdf = 0.5;
      return delta_f(wo, wi, n, nullptr, sample);
}

Spectrum FlatSurface::f(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const GlossSurface* out_material) const {
      if (out_material == nullptr) {
            LOG(WARNING) << "Calling f while transmitting from flat material to vacuum";
            return 0;
      }
      // both reflection and transmission will be delegated to GlossSurface::f
      out_material->f(wo, wi, Normal3f(n), this);
      
}