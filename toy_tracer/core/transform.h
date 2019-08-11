#pragma once
#include "core/common.h"
#include "core/ray.h"
struct Matrix4 {
      Float m_matrix[4][4];
      Matrix4();
      Matrix4(Float t00, Float t01, Float t02, Float t03, Float t10,
            Float t11, Float t12, Float t13, Float t20, Float t21,
            Float t22, Float t23, Float t30, Float t31, Float t32,
            Float t33);
      Matrix4(Float(*m)[4]) { memcpy(m_matrix, m, 4 * 4 * sizeof(Float)); }
      Float* operator[](const int n) { return m_matrix[n]; }
      const Float* operator[](const int n) const { return m_matrix[n]; }
      Matrix4 operator*(const Matrix4& m) const;
};

extern Matrix4 Inverse(const Matrix4 &m);

struct Transform {
      Matrix4 m;
      Transform* tInv = nullptr;
      Transform() = default;
      Transform(Float m[4][4]) : m(m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]) {}
      Transform(Matrix4& m) : m(m) {}
      Transform Inverse() { return Transform(::Inverse(m)); }
      void Inverse(Transform* _tInv) { *_tInv = ::Inverse(m); }
      void setInverse(Transform* _tInv) { tInv = _tInv; }
      const Matrix4* getMatrix() const { return &m; }
      Point3f operator() (const Point3f& o) const;
      Ray operator()(const Ray& r) const;
      Vector3f operator() (const Vector3f& o) const;
      Normal3f operator() (const Normal3f& o) const;
      Transform operator*(const Transform& rhs) const;
};

Transform Translate(const Vector3f &delta);

Transform Scale(Float x, Float y, Float z);