#include "light/skybox.h"
#include "main/renderworker.h"

Spectrum EquiRectSkybox::Li(Vector3f& w) const {
      Float u = SphericalTheta(w) / Pi;
      Float v = SphericalPhi(w) / 2.f / Pi;
      return _texture->Evaluate(u, v);
}


bool EquiRectSkybox::Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pdf) const {

      if (pdf) *pdf = 1.f / 4 / Pi;
      // TODO: if only transmissive or reflective material is found,
      // only sample hemisphere. Then pdf needs to be changed to 1/2Pi
      wi = SampleUnitSphere(sample);
      return true;
}

static Point3f _worldPositions[] = {
      {-1.0f,  -1.0f, -1.0f},
      {-1.0f, -1.0f, 1.0f},
      {-1.0f, 1.0f, -1.0f},
      {-1.0f, 1.0f, 1.0f},
      {1.0f, -1.0f, -1.0f},
      {1.0f, -1.0f, 1.0f},
      {1.0f, 1.0f, -1.0f},
      {1.0f, 1.0f, 1.0f}
};

static uint32_t indices[] = {
      1, 0, 4,
      1, 4, 5,
      4, 6, 5,
      5, 6, 7,
      6, 2, 3,
      6, 3, 7,
      0, 1, 2,
      2, 1, 3,
      0, 2, 6,
      0, 6, 4,
      3, 1, 5,
      3, 5, 7
};
static Layout skyboxLayout;
const std::string Skybox::default_path = "texture/skybox";
const std::vector<std::string> Skybox::default_files = { "right.tga", "left.tga", "up.tga", "down.tga", "back.tga", "front.tga" };

TriangleMesh Skybox::cube = TriangleMesh(indices, std::vector<LayoutItem>(1, DEFAULT_VERTEX_LAYOUT), 
      DEFAULT_VERTEX_LAYOUT.strip * 8, indices, 12, GL_UNSIGNED_INT, Transform::Identity());

void Skybox::glLoad()
{
      map.glLoad();
      cube.load();
}

void Skybox::draw()
{
      map.glUse();
      cube.glUse();
      Shader* s = LoadShader(SKY_BOX, true);
      s->use();
      s->setUniformF("rotation", &RenderWorker::getCamera()->world2cam().m);
      // set depth to 1
}
