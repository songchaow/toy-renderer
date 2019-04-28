#include "core/microfacet.h"

Float BeckmannDistribution::D(const Vector3f &wh) const {
      Float tan2Theta = Tan2Theta(wh);
      if (std::isinf(tan2Theta)) return 0.;
      Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
      return std::exp(-tan2Theta * (Cos2Phi(wh) / (alphax * alphax) +
            Sin2Phi(wh) / (alphay * alphay))) /
            (Pi * alphax * alphay * cos4Theta);
}
Float BeckmannDistribution::Lambda(const Vector3f &w) const {
      Float absTanTheta = std::abs(TanTheta(w));
      if (std::isinf(absTanTheta)) return 0.;
      // Compute _alpha_ for direction _w_
      Float alpha =
            std::sqrt(Cos2Phi(w) * alphax * alphax + Sin2Phi(w) * alphay * alphay);
      Float a = 1 / (alpha * absTanTheta);
      if (a >= 1.6f) return 0;
      return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

// wo and wi need to be normalized
// while wh is not required, if D(wh) is irrelevant with wh's length
Spectrum TorranceSparrow::f(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const FlatMaterial* out_material, bool exit) const {
    // determine whether reflection or refraction happens.
    if (Dot(wo, n) * Dot(wi, n) > 0) { // reflection
        const Vector3f wh = wo + wi;
        // theta_o, theta_i
        Float cosLocalWi = Dot(wi, wh) / wi.Length() / wh.Length();
        Float cosWo = AbsDot(wo, n);
        Float cosWi = AbsDot(wi, n);
        Spectrum fr;
        if (exit)
              fr = out_material->medium->Fr(cosLocalWi, medium.get());
        else fr = medium->Fr(cosLocalWi, out_material->medium);
        return distribution->G(wo, wi)*distribution->D(wh)*fr / cosWo / cosWi / 4;
    }
    else { // transmission
        if (out_material->medium->m_type == ObjectMedium::ObjectType::Metal) {
            LOG(WARNING) << "Light transmitted from metal.";
            return 0;
        }
        if (medium->m_type == ObjectMedium::ObjectType::Metal) {
            LOG(WARNING) << "Light transmitted into metal.";
            return 0;
        }
        const Dielectric* out_dielectric = dynamic_cast<const Dielectric*>(out_material->medium);
        const Dielectric* in_dielectric = dynamic_cast<const Dielectric*>(medium.get());
        if (exit) std::swap(out_dielectric, in_dielectric);
        Float eta_r = out_dielectric->GetEta() / in_dielectric->GetEta();
        const Vector3f wh = wo + wi* eta_r;
        Float cosWo = AbsDot(wo, n);
        Float cosWi = AbsDot(wi, n);
        Float cosLocalWi = Dot(wi, wh) / wi.Length() / wh.Length();
        Float wi_wh = AbsDot(wi, wh), wo_wh = AbsDot(wo, wh);
        Spectrum tmp = eta_r*eta_r*distribution->D(wh)*distribution->G(wo, wi)*(1 - medium->Fr(cosLocalWi, out_dielectric))*std::abs(wi_wh)*std::abs(wo_wh);
        Float div = (wi_wh + eta_r * wo_wh)*(wi_wh + eta_r * wo_wh)*cosWo*cosWi;
        return tmp / div;

    }
    ;
}