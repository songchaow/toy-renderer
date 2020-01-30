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

TriangleMesh Skybox::cube = TriangleMesh(_worldPositions, std::vector<LayoutItem>(1, DEFAULT_VERTEX_LAYOUT),
      DEFAULT_VERTEX_LAYOUT.strip * 8, indices, 12, GL_UNSIGNED_INT, Transform::Identity());

void Skybox::glLoad()
{
      if(map.ready2Load())
            map.glLoad();
      cube.load();
}

void Skybox::draw()
{
      glActiveTexture(GL_TEXTURE0);
      map.glUse();
      cube.glUse();
      Shader* s = LoadShader(SKY_BOX, true);
      s->use();
      Matrix4 rotation = RenderWorker::getCamera()->world2cam().m;
      Matrix4 cam2ndc = RenderWorker::getCamera()->Cam2NDC().m;
      rotation[0][3] = rotation[1][3] = rotation[2][3] = 0.f;
      s->setUniformF("rotation", &rotation);
      s->setUniformF("cam2ndc", &cam2ndc);
      s->setUniformI("skybox", 0);
      glDrawElements(GL_TRIANGLES, cube.face_count()*3, GL_UNSIGNED_INT, 0);
}
