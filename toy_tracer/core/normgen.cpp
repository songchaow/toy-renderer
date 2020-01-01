// Functions used to generate normal vectors from mesh
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

GLuint GenFaceNormal_GPU(const TriangleMesh& mesh, QOpenGLFunctions_4_0_Core* f) {
      GLuint _vao;
      f->glGenVertexArrays(1, &_vao);
      f->glBindVertexArray(_vao);
      // collect face into a texture
      GLuint faceIdxObj;
      f->glGenBuffers(1, &faceIdxObj);
      f->glBindBuffer(GL_ARRAY_BUFFER, faceIdxObj);
      f->glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(uint32_t) * mesh.face_count(), mesh.face_triangle(0), GL_STATIC_DRAW);
      // collect vertex points only
      Point3f* vertex_data = new Point3f[mesh.vertex_count()];
      const Layout& vertexLayout = mesh.layout();
      const LayoutItem* arrayLayoutItem = vertexLayout.getLayoutItem(ArrayType::ARRAY_VERTEX);
      assert(arrayLayoutItem);
      for (uint32_t i = 0; i < mesh.vertex_count(); i++) {
            const void* array_ptr = static_cast<const char*>(mesh.vertexData(i)) + arrayLayoutItem->offset;
            std::memcpy(vertex_data + i, array_ptr, arrayLayoutItem->e_count*arrayLayoutItem->e_size);
      }


      GLuint vertexPosObj;
      f->glGenBuffers(1, &vertexPosObj);
      f->glBindBuffer(GL_ARRAY_BUFFER, vertexPosObj);
      f->glBufferData(GL_ARRAY_BUFFER, arrayLayoutItem->e_count*arrayLayoutItem->e_size*mesh.vertex_count(), vertex_data, GL_STATIC_DRAW);
      GLuint vertexPosTexObj;
      f->glGenTextures(1, &vertexPosTexObj);
      f->glActiveTexture(GL_TEXTURE0);
      f->glBindTexture(GL_TEXTURE_BUFFER, vertexPosTexObj);
      // The internal image format should matter here
      f->glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vertexPosObj);


      // Create the receiving buffer
      GLuint faceNormObj;
      f->glGenBuffers(1, &faceNormObj);
      f->glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, faceNormObj);
      // allocate space for faceNomObj
      f->glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 3 * sizeof(Float)*mesh.face_count(), NULL, GL_DYNAMIC_COPY);
      f->glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, faceNormObj);
      
      // Shader and varyings
      Shader* normGenShader = LoadShader("shader/faceNormalGen.vs", "", nullptr); // do not link first
      normGenShader->setTransformFeedback(std::vector<std::string>(1, "faceNormal"));
      normGenShader->compileAndLink(f);
      normGenShader->use();
      normGenShader->setUniformI("posSampler", 0);
      // configure vertex attribute pointer
      f->glBindBuffer(GL_ARRAY_BUFFER, faceIdxObj);
      f->glVertexAttribIPointer(0, 3, GL_UNSIGNED_INT, 3 * sizeof(uint32_t), (void*)0);
      f->glEnableVertexAttribArray(0);
      f->glBeginTransformFeedback(GL_POINTS);
      // Draw
      f->glDrawArrays(GL_POINTS, 0, mesh.face_count());
      GLenum res = f->glGetError();
      f->glEndTransformFeedback();
      delete vertex_data;
      f->glDeleteBuffers(1, &faceIdxObj);
      f->glDeleteBuffers(1, &vertexPosObj);
      f->glDeleteTextures(1, &vertexPosTexObj);
      f->glBindVertexArray(0);
      //// debug
      //GLuint queryObj;
      //f->glGenQueries(1, &queryObj);
      //f->glBeginQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0, queryObj);
      //
      //GLint result;
      //f->glGetQueryObjectiv(queryObj, GL_QUERY_RESULT, &result);
      //LOG(INFO) << "feedback primitives:" << result;
      //f->glEndQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0);
      //f->glDeleteQueries(1, &queryObj);
      return faceNormObj;
}