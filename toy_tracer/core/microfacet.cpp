#include "core/microfacet.h"

Float BeckmannDistribution::D(const Vector3f &wh) const {
      Float tan2Theta = Tan2Theta(wh);
      if (std::isinf(tan2Theta)) return 0.;
      Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
      return std::exp(-tan2Theta * (Cos2Phi(wh) / (alpha * alpha) +
            Sin2Phi(wh) / (alpha * alpha))) /
            (Pi * alpha * alpha * cos4Theta);
}
Float BeckmannDistribution::D(const Float tan2Theta) const
{
      if (std::isinf(tan2Theta)) return 0.;
      Float divcos2Theta = 1 + tan2Theta;
      Float divcos4Theta = divcos2Theta * divcos2Theta;
      //Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
      /*return std::exp(-tan2Theta * (Cos2Phi(wh) / (alpha * alpha) +
            Sin2Phi(wh) / (alpha * alpha))) /
            (Pi * alpha * alpha * cos4Theta);*/
      return std::exp(-tan2Theta / alpha / alpha) / Pi / (alpha*alpha)*divcos4Theta;
}
Point2f BeckmannDistribution::Sample_wh(const Point2f & sample, Float * pdf) const
{
      Float phi = sample[0] * 2 * Pi;
      Float tan2theta = -alpha * alpha* std::log(sample[1]);
      if (pdf) *pdf = Pdf_wh({ phi,tan2theta });
      return { phi,tan2theta };
}
Float BeckmannDistribution::Pdf_wh(const Point2f & phi_tan2) const
{
      Float divcos2Theta = 1 + phi_tan2[1];
      Float divcosTheta = std::sqrt(divcos2Theta);
      return std::exp(-phi_tan2[1] / alpha / alpha) / Pi / (alpha*alpha)*divcos2Theta*divcosTheta;
}
Float BeckmannDistribution::Lambda(const Vector3f &w) const {
      Float absTanTheta = std::abs(TanTheta(w));
      if (std::isinf(absTanTheta)) return 0.;
      // Compute _alpha_ for direction _w_
      Float alpha =
            std::sqrt(Cos2Phi(w) * alpha * alpha + Sin2Phi(w) * alpha * alpha);
      Float a = 1 / (alpha * absTanTheta);
      if (a >= 1.6f) return 0;
      return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

// wo and wi need to be normalized, but not necessary to be in the correct frame
// while wh is not required, if D(wh) is irrelevant with wh's length
Spectrum TorranceSparrow::f(const Vector3f& wo, const Vector3f& wi, const Normal3f& n, const FlatMaterial* out_material) const {
    bool exit = SameOpposition(n, wi);
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
        else
              fr = medium->Fr(cosLocalWi, out_material->medium);
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
        const Dielectric* out_dielectric = static_cast<const Dielectric*>(out_material->medium);
        const Dielectric* in_dielectric = static_cast<const Dielectric*>(medium.get());
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

Spectrum TorranceSparrow::sample_f(const Point2f & random, const Vector3f & wo, Vector3f & wi, const FlatMaterial * out_material, Float* pdf) const
{
      // wo and wi should be in the correct coordinate!
      // sample wh
      Float p_wh;
      Point2f theta2phi = distribution->Sample_wh(random, &p_wh);
      Vector3f wh = Sphere2Vector(Point2f(std::atan(theta2phi[0]),theta2phi[1]));
      wi = Reflect(wo, wh);
      if (pdf)
            *pdf = PdfFromWh2Wi(p_wh, wh, wo);
      // evaluate f given wh
      return f(wo, wi, Normal3f(0,0,1), out_material);
}
