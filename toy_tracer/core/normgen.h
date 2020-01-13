
#pragma once
#include "shape/triangle.h"
#include <QOpenGLFunctions_4_0_Core>

void GenFaceNormal(const TriangleMesh& mesh);
GLuint GenFaceNormal_GPU(const TriangleMesh& mesh, QOpenGLFunctions_4_0_Core* f); 