#include "core/primitive.h"
#include "shape/triangle.h"
#include <QOpenGLFunctions_4_0_Core>

// generate a normal per face
void GenFaceNormal(const TriangleMesh& mesh) {
      std::vector<Normal3f> normals;
      for (int facei = 0; facei < mesh.face_count(); facei++) {
            const uint32_t* idx_array = mesh.face_triangle(facei);
            
            Vector3f v1;
      }
}

void GenFaceNormal_GPU(const TriangleMesh& mesh, QOpenGLFunctions_4_0_Core* f) {
      // collect face into a texture
      GLuint faceIdxObj;
      f->glGenBuffers(1, &faceIdxObj);
      f->glBindBuffer(GL_ARRAY_BUFFER, faceIdxObj);
      f->glBufferData(GL_ARRAY_BUFFER, mesh.face_count(), mesh.face_triangle(0), GL_STATIC_DRAW);
      // collect vertex points only
      RGBSpectrum* vertex_data = new RGBSpectrum[mesh.vertex_count()];
      for (uint32_t i = 0; i < mesh.vertex_count(); i++) {

      }
      delete vertex_data;
}