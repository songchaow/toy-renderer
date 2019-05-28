#include "core/material.h"
#include "core/geometry.h"
#include "core/spectrum.h"
#include <cassert>
#include <algorithm>
#include "core/common.h"

Spectrum Dielectric::Fr(Float cosWi, const ObjectMedium* out_medium, Float cosWt/* = 0*/) const
{
      Float eta_i;
      if (out_medium == nullptr)
            eta_i = 1.f;
      if (out_medium && out_medium->m_type == ObjectMedium::Dielectric)
            eta_i = dynamic_cast<const Dielectric*>(out_medium)->eta;
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





Spectrum FlatMaterial::delta_f(const Vector3f& wo, Vector3f & wi, const Vector3f& n, SimpleMaterial* out_material, bool reflect) const
{
    // wo should be in the same half sphere with n
    if (out_material && out_material->m_surface == SimpleMaterial::Gloss) {
        LOG(WARNING) << "Transmissing from gloss material";
        return 0.f;
    }
    Float cosWo = Dot(n, wo);
    // Calculate medium, eta_in and eta_out
    const Dielectric* out_medium = nullptr;
      Float eta_in, eta_out;
      if (out_material) {
            // gloss material shouldn't be processed in this function
            FlatMaterial* flat_out = static_cast<FlatMaterial*>(out_material);
            // out_material can't be other materials
            if (flat_out->medium->m_type == ObjectMedium::Metal) {
                  LOG(WARNING) << "Tranmissing from metal to electric.";
                  return 0.f;
            }
            out_medium = dynamic_cast<const Dielectric*>(flat_out->medium);
            eta_out = out_medium->GetEta();
      }
      else eta_out = 1.f;
      const Dielectric* din = dynamic_cast<const Dielectric*>(medium);
      Float eta_in = din->GetEta();
    if (reflect)
    {
        // wo known; wi=wo; calculate wt;
        wi = Reflect(wo, n);
        Vector3f wt;
        Refract(wi, n, eta_in / eta_out, wt);
        Float cosThetaI = Dot(wi, n);
        // wt is in the other half sphere, so revert the returned value
        Float cosThetaT = -Dot(wt, n);
        auto fr = medium->Fr(cosThetaI, out_medium, cosThetaT);
        return fr / cosThetaI;
    }
    else
    {
        // wt known; wo=wt; calculate wi;
        Refract(wo, n, eta_out / eta_in, wi);
        Float cosThetaI = std::abs(Dot(wi, n));
        auto fr = medium->Fr(cosThetaI, out_medium, cosWo);
        return (- fr + 1.f) / cosThetaI * eta_in*eta_in / eta_out / eta_out;
    }
}

Spectrum FlatMaterial::f(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const GlossMaterial* out_material) const {
      if (out_material == nullptr) {
            LOG(WARNING) << "Calling f while transmitting from flat material to vacuum";
            return 0;
      }
      // both reflection and transmission will be delegated to GlossMaterial::f
      out_material->f(wo, wi, n, this);
      
}