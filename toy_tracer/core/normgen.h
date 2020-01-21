
#pragma once
#include "shape/triangle.h"
#include <glad/glad.h>

void GenFaceNormal(const TriangleMesh& mesh);
GLuint GenFaceNormal_GPU(const TriangleMesh& mesh); 