#pragma once
#include "core/common.h"
#include "tracer/ray.h"
#include "core/geometry.h"
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
      Point3f translation;
      Float rotationX = 0.f, rotationY = 0.f, rotationZ = 0.f;
      Float scaleX = 1.f, scaleY = 1.f, scaleZ = 1.f;
      Matrix4 toMatrix4();
      static SRT fromTranslation(const Point3f& t) { SRT ret; ret.translation = t; return ret; }
      static SRT fromTranslation(Float x, Float y, Float z) {
            SRT ret; 
            ret.translation.x = x;
            ret.translation.y = y;
            ret.translation.z = z;
            return ret;
      }
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
      Vector3f operator()(const Vector3f& v) const;
      Point3f operator()(const Point3f& p) const;
      AABB operator() (const AABB& aabb) const;
      SRT toSRT() const;
      void transpose();
      static const Matrix4& Identity();
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
      AABB operator() (const AABB& aabb) const;
      Transform operator*(const Transform& rhs) const;

      static Transform Identity() { return Scale(1.f, 1.f, 1.f); }
};

// used by track sphere
struct AnimatedTransform : public Transform {
      SRT srt;
      AnimatedTransform(SRT srt) : srt(srt), Transform(srt.toMatrix4()) {}
      AnimatedTransform(const Matrix4& m) : srt(m.toSRT()), Transform(m) {}
      AnimatedTransform() = default;
      AnimatedTransform(const Transform& t) : AnimatedTransform(t.m) {}
      void rotate(Float angleX, Float angleY, Float angleZ) {
            // modify srt and update Transform
            srt.rotationX += angleX;
            srt.rotationY += angleY;
            srt.rotationZ += angleZ;
            m = srt.toMatrix4();
            // TODO: implement srt.toInvMatrix4()
            mInv = ::Inverse(m);
      }
      void move(Float offX, Float offY, Float offZ) {
            srt.translation.x += offX; srt.translation.y += offY; srt.translation.z += offZ;
            m = srt.toMatrix4();
            mInv = ::Inverse(m);
      }
      void move(Vector3f offset) {
            srt.translation.x += offset.x; srt.translation.y += offset.y; srt.translation.z += offset.z;
            m = srt.toMatrix4();
            mInv = ::Inverse(m);
      }
      void moveTo(Float x, Float y, Float z) {
            srt.translation.x = x; srt.translation.y = y; srt.translation.z = z;
            m = srt.toMatrix4();
            mInv = ::Inverse(m);
      }
      void update() { m = srt.toMatrix4(); mInv = ::Inverse(m); }
      Point3f& translation() { return srt.translation; }
      Point3f pos() { return srt.translation; }
};

struct Quaternion {
      // a + bi + cj + dk
      Float a; // also s
      Float b;
      Float c;
      Float d;
      Quaternion() = default;
      Quaternion(Float a, Float b, Float c, Float d)
            : a(a), b(b), c(c), d(d) {}
      Quaternion(Vector3f spinAxis, Float theta) {
            Normalize(spinAxis);
            a = std::cos(theta);
            Float sinT = std::sin(theta);
            b = sinT * spinAxis.x;
            c = sinT * spinAxis.y;
            d = sinT * spinAxis.z;
      }
      Quaternion operator*(const Quaternion& rhs) {
            const Float& e = rhs.a;
            const Float& f = rhs.b;
            const Float& g = rhs.c;
            const Float& h = rhs.d;
            Float newa = a * e - b * f - c * g - d * h;
            Float newb = a * f + b * e + c * h - d * g;
            Float newc = a * g - b * h + c * e + d * f;
            Float newd = a * h + b * g - c * f + d * e;
            Normalize(newa, newb, newc, newd);
            return Quaternion(newa, newb, newc, newd);
      }
      Quaternion Inverse() {
            // assume the quaternion normalized?
            return Quaternion(a, -b, -c, -d);
      }
      Matrix4 toMatrix4() {
            return Matrix4(a*a + b * b - c * c - d * d, 2 * b*c - 2 * a*d, 2 * b*d + 2 * a*c, 0,
                           2 * b*c + 2 * a*d, a*a - b * b + c * c - d * d, 2 * c*d - 2 * a*b, 0,
                           2 * b*d - 2 * a*c, 2 * c*d + 2 * a*b, a*a - b * b - c * c + d * d, 0,
                           0, 0, 0, 1);
      }
      Vector3f operator*(const Vector3f& rhs) {
            Quaternion rhs_q(0, rhs.x, rhs.y, rhs.z);
            Quaternion res = *this * rhs_q * Inverse();
            return Vector3f(res.b, res.c, res.d);
      }

};