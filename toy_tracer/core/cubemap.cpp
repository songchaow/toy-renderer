#include "core/cubemap.h"

/*static*/ const Transform CubeMap::camtoNDC { toNDCPerspective(0.1f, 1000.f, 1.f, 45 / Pi) };
/*static*/ const Transform CubeMap::o2cam[6] = { LookAt };