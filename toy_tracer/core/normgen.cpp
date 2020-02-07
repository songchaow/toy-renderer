// Functions used to generate normal vectors from mesh
#include "core/primitive.h"
#include "shape/triangle.h"
#include <glad/glad.h>

// generate a normal per face
void GenFaceNormal(const TriangleMesh& mesh) {
      std::vector<Normal3f> normals;
      for (int facei = 0; facei < mesh.face_count(); facei++) {
            const char* idx_array = mesh.face_triangle(facei);

            Vector3f v1;
      }
}

GLuint GenFaceNormal_GPU(const TriangleMesh& mesh) {
      GLuint _vao;
      glGenVertexArrays(1, &_vao);
      glBindVertexArray(_vao);
      // collect face into a texture
      GLuint faceIdxObj;
      glGenBuffers(1, &faceIdxObj);
      glBindBuffer(GL_ARRAY_BUFFER, faceIdxObj);
      Point3i* test_ptr = new Point3i[mesh.face_count()];
      for (int i = 0; i < mesh.face_count(); i++) {
            Point3i* view = test_ptr + i;
            std::memcpy(test_ptr + i, mesh.face_triangle(i), 3 * sizeof(uint32_t));
            //view->x = (1000 - i > 0) ? (1000-i) : 0;
            //view->y = 0;
            //view->z = 0;
            if (view->x < 0 || view->y < 0 || view->z < 0) {
                  int k = 5;
            }
      }
      //glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(uint32_t) * mesh.face_count(), mesh.face_triangle(0), GL_STATIC_DRAW);
      glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(uint32_t) * mesh.face_count(), test_ptr, GL_STATIC_DRAW);
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
      glGenBuffers(1, &vertexPosObj);
      glBindBuffer(GL_ARRAY_BUFFER, vertexPosObj);
      glBufferData(GL_ARRAY_BUFFER, arrayLayoutItem->e_count*arrayLayoutItem->e_size*mesh.vertex_count(), vertex_data, GL_STATIC_DRAW);
      GLuint vertexPosTexObj;
      glGenTextures(1, &vertexPosTexObj);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_BUFFER, vertexPosTexObj);
      // The internal image format should matter here
      glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vertexPosObj);


      // Create the receiving buffer
      GLuint faceNormObj;
      glGenBuffers(1, &faceNormObj);
      // allocate space for faceNormObj
      Point3f* p_data = new Point3f[mesh.face_count()];
      for (int i = 0; i < mesh.face_count(); i++) {
            p_data[i] = Point3f(0.f, 0.f, 1.f);
      }
      glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, faceNormObj);
      glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 3 * sizeof(Float)*mesh.face_count(), NULL, GL_DYNAMIC_COPY);
      //glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, faceNormObj, 0, 3 * sizeof(uint32_t) * mesh.face_count());
      glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, faceNormObj);

      // Shader and varyings
      Shader* normGenShader = LoadShader("shader/faceNormalGen.vs", "", false); // do not link first
      normGenShader->setTransformFeedback(std::vector<std::string>(1, "faceNormal"));
      normGenShader->compileAndLink();
      normGenShader->use();
      normGenShader->setUniformI("posSampler", 0);
      // configure vertex attribute pointer
      glBindBuffer(GL_ARRAY_BUFFER, faceIdxObj);
      //glVertexAttribPointer(0, 3, GL_UNSIGNED_INT, GL_FALSE, 3 * sizeof(uint32_t), (void*)0);
      glVertexAttribIPointer(0, 3, GL_UNSIGNED_INT, 3 * sizeof(uint32_t), (void*)0);
      glEnableVertexAttribArray(0);
      glBeginTransformFeedback(GL_POINTS);
      // Draw
      glDrawArrays(GL_POINTS, 0, mesh.face_count());
      GLenum res = glGetError();
      glEndTransformFeedback();
      delete vertex_data;
      glDeleteBuffers(1, &faceIdxObj);
      glDeleteBuffers(1, &vertexPosObj);
      glDeleteTextures(1, &vertexPosTexObj);
      glBindVertexArray(0);
      //// debug
      //GLuint queryObj;
      //glGenQueries(1, &queryObj);
      //glBeginQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0, queryObj);
      //
      //GLint result;
      //glGetQueryObjectiv(queryObj, GL_QUERY_RESULT, &result);
      //LOG(INFO) << "feedback primitives:" << result;
      //glEndQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, 0);
      //glDeleteQueries(1, &queryObj);
      return faceNormObj;
} 