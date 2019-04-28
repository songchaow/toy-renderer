#include "core/transform.h"

Matrix4::Matrix4() {
      m_matrix[0][0] = m_matrix[1][1] = m_matrix[2][2] = m_matrix[3][3] = 1.f;
      m_matrix[0][1] = m_matrix[0][2] = m_matrix[0][3] = m_matrix[1][0] = m_matrix[1][2] = m_matrix[1][3] = m_matrix[2][0] =
            m_matrix[2][1] = m_matrix[2][3] = m_matrix[3][0] = m_matrix[3][1] = m_matrix[3][2] = 0.f;
}

Matrix4::Matrix4(Float t00, Float t01, Float t02, Float t03, Float t10,
      Float t11, Float t12, Float t13, Float t20, Float t21,
      Float t22, Float t23, Float t30, Float t31, Float t32,
      Float t33) {
      m_matrix[0][0] = t00;
      m_matrix[0][1] = t01;
      m_matrix[0][2] = t02;
      m_matrix[0][3] = t03;
      m_matrix[1][0] = t10;
      m_matrix[1][1] = t11;
      m_matrix[1][2] = t12;
      m_matrix[1][3] = t13;
      m_matrix[2][0] = t20;
      m_matrix[2][1] = t21;
      m_matrix[2][2] = t22;
      m_matrix[2][3] = t23;
      m_matrix[3][0] = t30;
      m_matrix[3][1] = t31;
      m_matrix[3][2] = t32;
      m_matrix[3][3] = t33;
}

Matrix4 Matrix4::operator*(const Matrix4& m) {
      Matrix4 ret;
      for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                  ret[i][j] = 0.0f;
                  for (int t = 0; t < 4; t++)
                        ret[i][j] += m_matrix[i][t] * m[t][j];
            }
      return ret;
}

Point3f Transform::operator() (const Point3f& o) const {
      Point3f ret;
      ret.x = m[0][0] * o[0] + m[0][1] * o[1] + m[0][2] * o[2] + m[0][3];
      ret.y = m[1][0] * o[0] + m[1][1] * o[1] + m[1][2] * o[2] + m[1][3];
      ret.z = m[2][0] * o[0] + m[2][1] * o[1] + m[2][2] * o[2] + m[2][3];
      return ret;
}

Vector3f Transform::operator() (const Vector3f& o) const {
      Vector3f ret;
      ret.x = m[0][0] * o[0] + m[0][1] * o[1] + m[0][2] * o[2];
      ret.y = m[1][0] * o[0] + m[1][1] * o[1] + m[1][2] * o[2];
      ret.z = m[2][0] * o[0] + m[2][1] * o[1] + m[2][2] * o[2];
      return ret;
}

Ray Transform::operator()(const Ray& r) const {
      Ray ret;
      ret.o = operator()(r.o);
      ret.d = operator()(r.d);
      return ret;
}
