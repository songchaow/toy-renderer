#pragma once
#include "core/common.h"
#include "core/material.h"
#include "core/geometry.h"

class MicrofacetDistribution {
  public:
    virtual ~MicrofacetDistribution() {}
    virtual Float D(const Vector3f &wh) const = 0;
    virtual Float Lambda(const Vector3f &w) const = 0;
    virtual Point2f Sample_wh(const Point2f& sample, Float* pdf) const = 0;
    // Probability distribution of omega_h
    virtual Float Pdf_wh(const Point2f& phi_tan2) const = 0;
    Float G1(const Vector3f &w) const {
        //    if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
        return 1 / (1 + Lambda(w));
    }
    virtual Float G(const Vector3f &wo, const Vector3f &wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi));
    }
};

class BeckmannDistribution : public MicrofacetDistribution {
public:
      // BeckmannDistribution Public Methods
      static Float RoughnessToAlpha(Float roughness) {
            roughness = std::max(roughness, (Float)1e-3);
            Float x = std::log(roughness);
            return 1.62142f + 0.819955f * x + 0.1734f * x * x +
                  0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
      }
      BeckmannDistribution(Float alpha, bool samplevis = true)
            : alpha(alpha) {}
      Float D(const Vector3f &wh) const override;
      Float D(const Float tan2Thetah) const;

      virtual Point2f Sample_wh(const Point2f& sample, Float* pdf) const override;
      virtual Float Pdf_wh(const Point2f& phi_tan2) const override;

private:
      // BeckmannDistribution Private Methods
      Float Lambda(const Vector3f &w) const;

      // BeckmannDistribution Private Data
      const Float alpha;
};



class TorranceSparrow : public GlossSurface
{
    const std::shared_ptr<const ObjectMedium> medium;
    const std::shared_ptr<const MicrofacetDistribution> distribution;
public:
    TorranceSparrow(const Spectrum& R, const ObjectMedium* medium,const MicrofacetDistribution* dist) :GlossSurface(R),medium(medium),distribution(dist) {}
    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi, const Normal3f& n, const FlatSurface* out_material) const override;
    virtual Spectrum sample_f(const Point2f& random, const Vector3f& wo, Vector3f& wi, const FlatSurface* out_material, Float *pdf) const override;
};