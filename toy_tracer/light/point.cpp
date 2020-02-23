#include "light/point.h"

bool PointLight::Sample_wi(Point2f& sample, Interaction& i, Vector3f& wi, Float* pwi) const {
      return true;
}

// assume the framebuffer and its attachment are correct
void PointLight::drawDepthMap_Point(const View& v) const
{
      Shader* s = LoadShader(DEPTH_MAP, true);
      s->setUniformF("world2cam", &v.world2view);
      Matrix4 cam2ndc = v.f.cam2ndc_Orthogonal();
      s->setUniformF("cam2ndc", &cam2ndc);
      s->setUniformF("far", v.f.Far);

}

void PointLight::CreateDepthMap_Direction(const View& v) const
{
      // v's direction is the same as the light's direction
      Shader* s = LoadShader(DEPTH_MAP, true);
      s->setUniformF("world2cam", &v.world2view);
      Matrix4 cam2ndc = v.f.cam2ndc_Orthogonal();
      s->setUniformF("cam2ndc", &cam2ndc);

}
