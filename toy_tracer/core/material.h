#pragma once

#include "core/common.h"
#include "core/geometry.h"
#include <memory>
#include <algorithm>
#include "core/spectrum.h"

/*  Medium
    defines how light changes its strength and orientation
    when comes in or out of medium
*/
class ObjectMedium
{
public:
    enum ObjectType {Dielectric, Metal};
    const ObjectType m_type;
    ObjectMedium(const ObjectType type) : m_type(type) {}
    virtual Spectrum Fr(Float cosWi, const ObjectMedium* out_medium, Float cosWt = 0) const = 0;
};

class SimpleMaterial
{
    
public:
      enum SurfaceType {Flat, Gloss};
      SimpleMaterial(const SurfaceType m_type, const Spectrum& R) :m_surface(m_type), R(R) {}

      const SurfaceType m_surface;
private:
      const Spectrum R;
};

class GlossMaterial;
class FlatMaterial : public SimpleMaterial
{
public:
    const ObjectMedium* medium;
    FlatMaterial(const ObjectMedium* medium, const Spectrum& R) :medium(medium), SimpleMaterial(SimpleMaterial::SurfaceType::Flat, R) {}
    // wo, wi, n should be normalized
    Spectrum delta_f(const Vector3f& wo, Vector3f & wi, const Vector3f& n, SimpleMaterial* out_material, bool reflect) const;
    // only valid when out_material is glossy
    Spectrum f(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const GlossMaterial* out_material) const;
};

class GlossMaterial : public SimpleMaterial
{
    
public:
    GlossMaterial(const Spectrum& R) : SimpleMaterial(Gloss, R) {}
    virtual Spectrum f(const Vector3f& wo, const Vector3f& wi, const Vector3f& n, const FlatMaterial* out_material, bool exit = false) const = 0;
    virtual Spectrum sample_f(const Point2f& random, const Vector3f& wo, Vector3f& wi, const Vector3f& n, const FlatMaterial* out_material) const = 0;
};

/*  Dielectric
    with eta constant for full spectrum field
 */
class Dielectric : public ObjectMedium
{
public:
    Dielectric(Float eta) : ObjectMedium(ObjectType::Dielectric), eta(eta) {}
    Float GetEta() const {return eta;}
    // `out` here just means the outter substance. Not means scatterring into
    virtual Spectrum Fr(Float cosWi, const ObjectMedium* out_medium, Float cosWt=0) const;
    Spectrum Fr_fromT(Float cosWo, Float cosWi, const ObjectMedium* out_medium) const;
private:
    Float eta;

};

/*  Metal
    with eta varying with lambda
 */
class Metal : public ObjectMedium
{
public:
    Metal(Spectrum& eta, Spectrum&k) :ObjectMedium(ObjectType::Metal),eta(eta),k(k) {}
    virtual Spectrum Fr(Vector3f wo, Float cosWo, const ObjectMedium* out_medium) const;
private:
    Spectrum eta;
    Spectrum k;
};

inline Float CosTheta(const Vector3f &w) { return w.z; }
inline Float Cos2Theta(const Vector3f &w) { return w.z * w.z; }
inline Float AbsCosTheta(const Vector3f &w) { return std::abs(w.z); }
inline Float Sin2Theta(const Vector3f &w) {
    return std::max((Float)0, (Float)1 - Cos2Theta(w));
}

inline Float SinTheta(const Vector3f &w) { return std::sqrt(Sin2Theta(w)); }

inline Float TanTheta(const Vector3f &w) { return SinTheta(w) / CosTheta(w); }

inline Float Tan2Theta(const Vector3f &w) {
    return Sin2Theta(w) / Cos2Theta(w);
}

inline Float CosPhi(const Vector3f &w) {
    Float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

inline Float SinPhi(const Vector3f &w) {
    Float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

inline Float Cos2Phi(const Vector3f &w) { return CosPhi(w) * CosPhi(w); }

inline Float Sin2Phi(const Vector3f &w) { return SinPhi(w) * SinPhi(w); }

inline Float CosDPhi(const Vector3f &wa, const Vector3f &wb) {
    return Clamp(
        (wa.x * wb.x + wa.y * wb.y) / std::sqrt((wa.x * wa.x + wa.y * wa.y) *
        (wb.x * wb.x + wb.y * wb.y)),
        -1, 1);
}

/*  Other material classes
    may have eta varying with lambda, AND texture coordinates
 */