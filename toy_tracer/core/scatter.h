#pragma once
#include <utility>
#include <vector>
#include "core/common.h"
#include "core/geometry.h"
#include "core/primitive.h"
#include "core/spectrum.h"

/*  SurfaceBoundary
    Created by intersection method
 */
struct SurfaceBoundary
{
    // when traced, light goes from out_p into in_p!
    Primitive3D* out_p;
    Primitive3D* in_p;
    Point3f p;
    Normal3f n;
    // TODO: texture coordinates may be needed.
    // Spectrum Reflect
    // Spectrum Transmit
};

class ScatterFunc
{
public:
    enum ScatterType {Discrete, Consecutive, Mix};
private:
    const ScatterType _type;
public:
    ScatterFunc(ScatterType type) : _type(type) { }
    bool isDiscrete() const {return _type==ScatterType::Discrete;}
    bool isConsecutive() const {return _type==ScatterType::Consecutive;}
    bool isMix() const {return _type==ScatterType::Mix;}
};

// ScatterItem: <ratio:Float, scattering direction:Vector3f>
using ScatterItem = std::pair<Spectrum,Vector3f>;

class DiscreteScatter : public ScatterFunc
{
public:
    DiscreteScatter() : ScatterFunc(ScatterType::Discrete) {}
    /* Discrete_f: Given input direction, return scattering directions and correspoinding radiance or flux.
    bool withLoCos: true if result has considered shrinking of radiance because of cosine
    bool withLiCos: true if input has considered shrinking of radiance because of cosine
    if both are false, Discrete_f gives a Fresnel reflecation rate.
    if LiCos==true, LoCos==false, Discrete_f gives a BxDF. */
    virtual void Discrete_f(std::vector<ScatterItem> &scatter_result, const Vector3f &wi, const Vector3f &n, bool &withLoCos, bool &withLiCos) = 0;

};

class ConsecutiveScatter : public ScatterFunc
{
    ConsecutiveScatter() : ScatterFunc(ScatterType::Consecutive) {}
    virtual void f(Spectrum& result, const Vector3f &wi, const Vector3f &wo, const Vector3f &n) = 0;

};