/* Specular reflection and transmission */
#pragma once

#include "core/scatter.h"

class SpecularReflect : public DiscreteScatter
{
public:
    SpecularReflect(Float eta_i, Float eta_o) :eta_i(eta_i),eta_o(eta_o) {}
    virtual void Discrete_f(std::vector<ScatterItem> &scatter_source, Vector3f &wi, const Vector3f &n, bool &withLoCos, bool &withLiCos) = 0;
private:
    Float eta_i;
    Float eta_o;
};

class DielectricReflect : public SpecularReflect
{
public:
    DielectricReflect(Float eta_i, Float eta_o) : SpecularReflect(eta_i,eta_o) {}
    virtual void Discrete_f(std::vector<ScatterItem> &scatter_source, Vector3f &wi, const Vector3f &n, bool &withLoCos, bool &withLiCos) override;
};