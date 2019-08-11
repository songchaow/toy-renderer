#include "core/common.h"
#include "core/shape.h"
#include "core/geometry.h"
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