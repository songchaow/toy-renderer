#include "shape/rectangular.h"
#include "shape/triangle.h"

std::vector<TriangleMesh*> Rectangular::GenMesh() const {
      Point3f pos[4] = {   {-width / 2, -height / 2, 0},  
                              {-width / 2, height / 2, 0}, 
                              {width / 2, -height / 2, 0} , 
                              {width / 2, height / 2, 0} };
      Point2f tex[4] = { {0.f, 0.f}, {0.f, 1.f}, {1.f, 0.f}, {1.f, 1.f} };
      Normal3f norm = { 0.f, 0.f, 1.f };
      Layout layout;
      // TODO: data_ptr is not needed anymore. Consider move it out of LayoutItem
      layout.emplace_back(ArrayType::ARRAY_VERTEX, GL_FLOAT, sizeof(Float), 3, false);
      layout.emplace_back(ArrayType::ARRAY_TEX_UV, GL_FLOAT, sizeof(Float), 2, false);
      layout.emplace_back(ArrayType::ARRAY_NORMAL, GL_FLOAT, sizeof(Float), 3, false);
      Float* vertex_data = new Float[layout.strip() * 4];
      for (int i = 0; i < 4; i++) {
            Float* p = vertex_data + i * layout.strip() / sizeof(Float);
            *(Point3f*)(p) = pos[i];
            p += 3;
            *(Point2f*)(p) = tex[i];
            p += 2;
            *(Normal3f*)(p) = norm;
      }
      uint32_t* index_data = new uint32_t[6]{ 1, 0, 2, 1, 2, 3 };
      // obj2world!
      TriangleMesh* ret = new TriangleMesh(vertex_data, layout, layout.strip() * 4, index_data, 2, GL_UNSIGNED_INT, Transform::Identity());
      return std::vector<TriangleMesh*>(1, ret);
}

bool Rectangular::Intercept(const Ray & r, Interaction & i) const
{
      Ray rLocal(world2obj(r));
      Float t = -rLocal.o.z / rLocal.d.z;
      Float x = rLocal.o.x + t * rLocal.d.x;
      if (x > width / 2 || x < -width / 2)
            return false;
      Float y = rLocal.o.y + t * rLocal.d.y;
      if (y > height / 2 || y < -height / 2)
            return false;
      i.pWorld = obj2world(Point3f(x, y, 0.f));
      i.dpdu = obj2world(Vector3f(width, 0.f, 0.f));
      i.dpdv = obj2world(Vector3f(0.f, -height, 0.f));
      i.n = obj2world(Normal3f(0.f, 0.f, 1.f));
      i.u = (x + width / 2) / width;
      i.v = 1.f- (y + height / 2) / height;
      i.wo = -r.d;
      return true;
}

bool Rectangular::InterceptP(const Ray& r, Interaction* i) const {
      Ray rLocal(world2obj(r));
      Float t = -rLocal.o.z / rLocal.d.z;
      Float x = rLocal.o.x + t * rLocal.d.x;
      if (x > width / 2 || x < -width / 2)
            return false;
      Float y = rLocal.o.y + t * rLocal.d.y;
      if (y > height / 2 || y < -height / 2)
            return false;
      return true;
}