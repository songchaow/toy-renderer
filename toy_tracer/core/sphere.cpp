#include "core/common.h"
#include "core/shape.h"
#include "core/geometry.h"
#include "shape/triangle.h"
#include <algorithm>

bool Sphere::Intercept(const Ray& r0, Interaction& i) const {
      Ray rt(world2obj(r0));
      Float a = rt.d.x*rt.d.x + rt.d.y*rt.d.y + rt.d.z*rt.d.z;
      Float b = 2 * (rt.d.x*rt.o.x + rt.d.y*rt.o.y + rt.d.z*rt.o.z);
      Float c = rt.o.x*rt.o.x + rt.o.y*rt.o.y + rt.o.z*rt.o.z - radius * radius;
      Float t0, t1, ti;
      if (!Quadratic(a, b, c, &t0, &t1))
            return false;
      if (t0*t1 <= 0) ti = std::max(t0, t1);
      else if (t0 > 0) ti = std::min(t0, t1);
      else return false; // both negative
      Point3f pHit(r0.Transmit(ti));
      Point3f pHitLocal(rt.Transmit(ti));
      Float theta = std::acos(Clamp(pHitLocal.z / radius, -1, 1));
      Float phi;
      if (pHitLocal.x == 0) phi = pHitLocal.y > 0 ? Pi / 2 : 3 * Pi / 2;
      else if (pHitLocal.y == 0) phi = pHitLocal.x > 0 ? 0 : Pi;
      else if (pHitLocal.x > 0) phi = std::atan(pHitLocal.y / pHitLocal.x);
      else phi = Pi + std::atan(pHitLocal.y / pHitLocal.x);
      i.pWorld = pHit;
      i.u = phi / 2 / Pi;
      i.v = theta / Pi;
      i.n = obj2world(Normal3f(Normalize(Vector3f(pHitLocal))));
      i.wo = -r0.d;
      i.dpdu = obj2world(Vector3f(2 * Pi*pHitLocal.y, 2 * Pi*pHitLocal.x, 0));
      i.dpdv = obj2world(Vector3f(Pi*pHitLocal.z*std::cos(phi), Pi*pHitLocal.z*std::sin(phi), -Pi * radius*std::sin(theta)));
      return true;
}

bool Sphere::InterceptP(const Ray& r0, Interaction* i) const {
      Ray rt(world2obj(r0));
      Float a = rt.d.x*rt.d.x + rt.d.y*rt.d.y + rt.d.z*rt.d.z;
      Float b = 2 * (rt.d.x*rt.o.x + rt.d.y*rt.o.y + rt.d.z*rt.o.z);
      Float c = rt.o.x*rt.o.x + rt.o.y*rt.o.y + rt.o.z*rt.o.z - radius * radius;
      Float t0, t1, ti;
      if (!Quadratic(a, b, c, &t0, &t1))
            return false;
      if (t0*t1 <= 0) ti = std::max(t0, t1);
      else if (t0 > 0) ti = std::min(t0, t1);
      else return false; // both negative
      if (ti <= MIN_DISTANCE) // avoid self interception
            return false;
      if (i)
            i->t = ti;
      return true;
}

bool Sphere::ComputeDiff(const Ray& r0, Interaction* i) const {
      Float& ti = i->t;
      Ray rt(world2obj(r0));
      Point3f pHit(r0.Transmit(ti));
      Point3f pHitLocal(rt.Transmit(ti));
      Float theta = std::acos(Clamp(pHitLocal.z / radius, -1, 1));
      Float phi;
      if (pHitLocal.x == 0) phi = pHitLocal.y > 0 ? Pi / 2 : 3 * Pi / 2;
      else if (pHitLocal.y == 0) phi = pHitLocal.x > 0 ? 0 : Pi;
      else if (pHitLocal.x > 0) phi = std::atan(pHitLocal.y / pHitLocal.x);
      else phi = Pi + std::atan(pHitLocal.y / pHitLocal.x);
      i->pWorld = pHit;
      i->u = phi / 2 / Pi;
      i->v = theta / Pi;
      i->n = obj2world(Normal3f(Normalize(Vector3f(pHitLocal))));
      i->wo = -r0.d; // assumed that d is normalized already
      i->dpdu = obj2world(Vector3f(2 * Pi*pHitLocal.y, 2 * Pi*pHitLocal.x, 0));
      i->dpdv = obj2world(Vector3f(Pi*pHitLocal.z*std::cos(phi), Pi*pHitLocal.z*std::sin(phi), -Pi * radius*std::sin(theta)));
      return true;
}

Point3f Sphere::PointfromUV(Float u, Float v, Normal3f * n) const
{
      Float theta = u * Pi, phi = v * Pi;
      Point3f localP(radius*std::sin(theta)*std::cos(phi), radius*std::sin(theta)*std::sin(phi), radius*std::cos(theta));
      Normal3f localN = Normal3f(Vector3f(localP / radius));
      if (n)
            *n = obj2world(localN);
      return obj2world(localP);
}



Vector3f SampleUnitSphere(const Point2f& sample) {
      Float z = 1 - 2 * sample[0];
      Float r = 2 * std::sqrt(sample[0] * (1 - sample[0]));
      Float x = r * std::cos(2 * Pi*sample[1]);
      Float y = r * std::sin(2 * Pi*sample[1]);
      return { x,y,z };
}

Point3f Sphere::SamplePoint(Point2f & random, Interaction & i, Normal3f & n, Float* pdf) const
{
      Vector3f p_n(SampleUnitSphere(random));
      n = obj2world((Normal3f)(p_n));
      if (pdf)
            *pdf = 1.f / Area();
      return obj2world(Point3f(p_n*radius));
}

std::vector<TriangleMesh*> Sphere::GenMesh(unsigned int uSlide, unsigned int vSlide) const {
      // First, generate all vertices
      // attributes: position, uv, normal, 
      Layout layout;
      // TODO: data_ptr is not needed anymore. Consider move it out of LayoutItem
      layout.emplace_back(ArrayType::ARRAY_VERTEX, GL_FLOAT, sizeof(Float), 3, false);
      layout.emplace_back(ArrayType::ARRAY_TEX_UV, GL_FLOAT, sizeof(Float), 2, false);
      layout.emplace_back(ArrayType::ARRAY_NORMAL, GL_FLOAT, sizeof(Float), 3, false);
      // one top, one bottom, and the remaining uSlide-1 lines
      uint32_t numVertices = (uSlide - 1)*vSlide + 2;
      void* vertex_data = new char[layout.strip()*numVertices];

      Float* vertex_data_ptr = static_cast<Float*>(vertex_data);
      for (unsigned int u = 1; u < uSlide; u++) {
            Float theta = Pi / (Float)uSlide * Float(u);
            Float z = radius - radius / uSlide * u;
            z = radius * std::cos(theta);
            Float uPos = std::acos(z / radius) / Pi;
            uPos = (Float)u / uSlide;
            Float radiusZ = std::sqrt(radius*radius - z * z);
            for (unsigned int v = 0; v < vSlide; v++) {
                  Float phi = 2 * Pi / vSlide * v;
                  Point3f pos(radiusZ*std::cos(phi), radiusZ*std::sin(phi), z);
                  Point2f uv(uPos, Float(v) / vSlide);
                  Point3f normal(std::cos(phi), std::sin(phi), z / radius);
                  *(Point3f*)vertex_data_ptr = pos;
                  vertex_data_ptr += 3;
                  *(Point2f*)vertex_data_ptr = uv;
                  vertex_data_ptr += 2;
                  *(Point3f*)vertex_data_ptr = normal;
                  vertex_data_ptr += 3;
            }
      }
      // Add the two top and bottom points
      Point3f topPos(0.f, 0.f, radius);
      Point2f topUV(0.f, 0.f);
      Normal3f topNormal(0.f, 0.f, 1.f);
      *(Point3f*)vertex_data_ptr = topPos;
      vertex_data_ptr += 3;
      *(Point2f*)vertex_data_ptr = topUV;
      vertex_data_ptr += 2;
      *(Normal3f*)vertex_data_ptr = topNormal;
      vertex_data_ptr += 3;
      Point3f bottomPos(0.f, 0.f, -radius);
      Point2f bottomUV(1.f, 0.f);
      Normal3f bottomNormal(0.f, 0.f, -1.f);
      *(Point3f*)vertex_data_ptr = bottomPos;
      vertex_data_ptr += 3;
      *(Point2f*)vertex_data_ptr = bottomUV;
      vertex_data_ptr += 2;
      *(Normal3f*)vertex_data_ptr = bottomNormal;
      vertex_data_ptr += 3;

      // Second: generate triangles in anticlockwise order
      const uint32_t faceNum = vSlide * (2 + 2 * (uSlide - 2));
      // the first layer
      uint32_t* idx_data = new uint32_t[3 * faceNum];
      uint32_t* idx_ptr = idx_data;
      uint32_t topPointIdx = numVertices - 2;
      uint32_t bottomPointIdx = numVertices - 1;
      for (uint32_t facei = 0; facei < vSlide; facei++) {
            *idx_ptr++ = topPointIdx;
            *idx_ptr++ = facei;
            *idx_ptr++ = (facei + 1) % vSlide;
      }
      // uSlide-2 layers in the middle; the [2,uSlide-1] layer: index from 0 to uSlide-3
      for (uint32_t layeri = 0; layeri < uSlide - 2; layeri++) {
            // faces from line layeri to line layeri+1
            uint32_t idxUpper = layeri * vSlide;
            uint32_t idxLower = idxUpper + vSlide;
            for (uint32_t offset = 0; offset < vSlide; offset++) {
                  auto offsetPlus = (offset + 1) % vSlide;
                  // face I
                  *idx_ptr++ = idxUpper + offset;
                  *idx_ptr++ = idxLower + offset;
                  *idx_ptr++ = idxLower + offsetPlus;
                  // face II
                  *idx_ptr++ = idxUpper + offset;
                  *idx_ptr++ = idxLower + offsetPlus;
                  *idx_ptr++ = idxUpper + offsetPlus;
            }
      }
      // the last layer
      uint32_t lastLineIdx = (uSlide - 2)*vSlide;
      for (uint32_t facei = 0; facei < vSlide; facei++) {
            *idx_ptr++ = lastLineIdx + facei;
            *idx_ptr++ = bottomPointIdx;
            *idx_ptr++ = lastLineIdx + (facei + 1) % vSlide;
      }
      TriangleMesh* m = new TriangleMesh(vertex_data, layout._data, layout.strip()*numVertices, 
            numVertices, idx_data, faceNum, GL_UNSIGNED_INT, Transform::Identity());
      return std::vector<TriangleMesh*>(1, m);
}