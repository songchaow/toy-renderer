#include "core/specular.h"
#include "core/common.h"

void DielectricReflect::Discrete_f(std::vector<ScatterItem> &scatter_source, Vector3f &wi, const Vector3f &n, bool &withLoCos, bool &withLiCos)
{
    DLOG_IF(WARNING,scatter_source.size()!=0) << "scatter_source not empty.";

}