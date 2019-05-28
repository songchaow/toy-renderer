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
      Matrix4 operator*(const Matrix4& m);
};

Matrix4 Inverse(const Matrix4 &m) {
      int indxc[4], indxr[4];
      int ipiv[4] = { 0, 0, 0, 0 };
      Float minv[4][4];
      memcpy(minv, m.m_matrix, 4 * 4 * sizeof(Float));
      for (int i = 0; i < 4; i++) {
            int irow = 0, icol = 0;
            Float big = 0.f;
            // Choose pivot
            for (int j = 0; j < 4; j++) {
                  if (ipiv[j] != 1) {
                        for (int k = 0; k < 4; k++) {
                              if (ipiv[k] == 0) {
                                    if (std::abs(minv[j][k]) >= big) {
                                          big = Float(std::abs(minv[j][k]));
                                          irow = j;
                                          icol = k;
                                    }
                              }
                              else if (ipiv[k] > 1)
                                    LOG(ERROR) << "Singular matrix in MatrixInvert";
                        }
                  }
            }
            ++ipiv[icol];
            // Swap rows _irow_ and _icol_ for pivot
            if (irow != icol) {
                  for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
            }
            indxr[i] = irow;
            indxc[i] = icol;
            if (minv[icol][icol] == 0.f) LOG(ERROR) << "Singular matrix in MatrixInvert";

            // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
            Float pivinv = 1. / minv[icol][icol];
            minv[icol][icol] = 1.;
            for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

            // Subtract this row from others to zero out their columns
            for (int j = 0; j < 4; j++) {
                  if (j != icol) {
                        Float save = minv[j][icol];
                        minv[j][icol] = 0;
                        for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
                  }
            }
      }
      // Swap columns to reflect permutation
      for (int j = 3; j >= 0; j--) {
            if (indxr[j] != indxc[j]) {
                  for (int k = 0; k < 4; k++)
                        std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
            }
      }
      return Matrix4(minv);
}

struct Transform {
      Matrix4 m;
      Transform(Float m[4][4]) : m(m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]) {}
      Transform(Matrix4& m) : m(m) {}
      Transform Inverse() { return Transform(::Inverse(m)); }
      Point3f operator() (const Point3f& o) const;
      Ray operator()(const Ray& r) const;
      Vector3f operator() (const Vector3f& o) const;
      Normal3f operator() (const Normal3f& o) const;

};

