#pragma once
#include "core/common.h"
#include "core/ray.h"
struct Matrix4;
struct Matrix3 {
      Float m_matrix[3][3];
      Matrix3();
      Matrix3(Float t00, Float t01, Float t02,
            Float t10, Float t11, Float t12,
            Float t20, Float t21, Float t22);
      Matrix3(const Matrix4& m4);
      Float* operator[](const int n) { return m_matrix[n]; }
      const Float* operator[](const int n) const { return m_matrix[n]; }
};
struct Transform;
struct SRT {
      Float translationX, translationY, translationZ;
      Float rotationX, rotationY, rotationZ;
      Float scaleX, scaleY, scaleZ;
      Matrix4 toMatrix4();
};

struct Matrix4 {
      Float m_matrix[4][4];
      Matrix4();
      Matrix4(Float t00, Float t01, Float t02, Float t03, Float t10,
            Float t11, Float t12, Float t13, Float t20, Float t21,
            Float t22, Float t23, Float t30, Float t31, Float t32,
            Float t33);
      Matrix4(Float(*m)[4]) { memcpy(m_matrix, m, 4 * 4 * sizeof(Float)); }
      // convert from Matrix3
      Matrix4(const Matrix3& m3);
      Float* operator[](const int n) { return m_matrix[n]; }
      const Float* operator[](const int n) const { return m_matrix[n]; }
      Matrix4 operator*(const Matrix4& m) const;
      SRT toSRT() const;
};

extern Matrix4 Inverse(const Matrix4 &m);



struct Transform;

Transform Translate(const Vector3f &delta);
Transform Translate(const Float x, const Float y, const Float z);
Matrix4 TranslateM(const Vector3f &delta);
Matrix4 TranslateM(const Float x, const Float y, const Float z);

Transform Scale(Float x, Float y, Float z);
Matrix4 ScaleM(Float x, Float y, Float z);

Transform Rotate(Float dTheta, Float dPhi);
Matrix4 RotateM(Float rx, Float ry, Float rz);

struct Transform {
      Matrix4 m;
      Matrix4 mInv;
      // tInv deprecated
      Transform* tInv = nullptr;
      Transform() = default;
      Transform(Float m[4][4]) : m(m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]) {
            mInv = ::Inverse(m);
      }
      Transform(Float a1, Float a2, Float a3, Float a4,
            Float b1, Float b2, Float b3, Float b4,
            Float c1, Float c2, Float c3, Float c4,
            Float d1, Float d2, Float d3, Float d4) :m(a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4) {
            mInv = ::Inverse(m);
      }
      Transform(const Matrix4& m) : m(m) { mInv = ::Inverse(m); }
      Transform(const Matrix4& m, const Matrix4& mInv) :m(m), mInv(mInv) {}
      Transform Inverse() const { return Transform(mInv, m); }
      // Following 2 are deprecated
      void Inverse(Transform* _tInv) { *_tInv = ::Inverse(m); }
      void setInverse(Transform* _tInv) { tInv = _tInv; }
      const Matrix4* getRowMajorData() const { return &m; }
      Point3f operator() (const Point3f& o) const;
      Ray operator()(const Ray& r) const;
      Vector3f operator() (const Vector3f& o) const;
      Normal3f operator() (const Normal3f& o) const;
      Transform operator*(const Transform& rhs) const;

      static Transform Identity() { return Scale(1.f, 1.f, 1.f); }
};