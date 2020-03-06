#include "core/frustum.h"
#include "core/sampler.h"
#include "main/renderworker.h"

static Sampler s;

void Frustum::randomShift_Perspective(Matrix4& cam2ndc) {
      Float oneOverWidth = 1.f / RenderWorker::Instance()->resolution().x;
      Float oneOverHeight = 1.f / RenderWorker::Instance()->resolution().y;
      Point2f shift = s.Sample2D() - 0.5f;
      cam2ndc[0][2] = oneOverWidth * shift.x * 2; // shift in ndc, [-0., 0.]
      cam2ndc[1][2] = oneOverHeight * shift.y * 2;
}