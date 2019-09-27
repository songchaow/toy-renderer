#include "core/primitive.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "main/renderworker.h"

Primitive* CreatePrimitiveFromMeshes(TriangleMesh* mesh) {
      Primitive* p = new Primitive(nullptr, nullptr);
      //p->setMesh(mesh);
      return p;
}
Primitive* CreatePrimitiveFromModelFile(std::string path) {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
      std::vector<TriangleMesh*> meshes = LoadMeshes(scene);
      // TODO: maybe other stuff(texture map info, etc.) could be load together
      Primitive* p = new Primitive(nullptr, nullptr);
      p->setMeshes(meshes);
      return p;
}

// TODO: move to main
void Primitive::addProperties(QWidget * parent) {
      RendererObject::addProperties(parent);
      QGroupBox* primitive_group = new QGroupBox("Primitive");
      // set current widget's layout
      QVBoxLayout* currentLayout = new QVBoxLayout;
      primitive_group->setLayout(currentLayout);
      // add toolbars for current item
      addConstText("another p:", "this value", primitive_group);
      // add sub toolbars for child items
      QGroupBox* shapeToolBox = new QGroupBox("Shape");
      if(shape)
            shape->addProperties(shapeToolBox);
      currentLayout->addWidget(shapeToolBox);
      parent->layout()->addWidget(primitive_group);
}

void Primitive::load(QOpenGLExtraFunctions* f) {
      for (auto* mesh : _meshes) {
            mesh->load(f);
      }
      // TODO: consider other types of RTMaterial
      if (rt_m == nullptr) {
            rt_m = new PBRMaterial();
            rt_m->moveToThread(thread());
      }
      rt_m->load(f);
      shader = rt_m->shader();
}

void Primitive::draw(QOpenGLExtraFunctions* f) {
      // draw all meshes
      // TODO: maybe different meshes' materials/textures are different.
      shader->use();

      if (rt_m->albedo_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE0);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->albedo_map.tbo());
            shader->setUniformI("albedoSampler", 0);
      }
      if (rt_m->metallic_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE1);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->metallic_map.tbo());
            shader->setUniformI("metallicSampler", 1);
      }
      if (rt_m->rough_map.isLoad()) {
            f->glActiveTexture(GL_TEXTURE2);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->rough_map.tbo());
            shader->setUniformI("roughnessSampler", 2);
      }
      
      for (auto& m : _meshes) {
            f->glBindVertexArray(m->vao());
            // set MVP
            // TODO: obj2world could be stored for each mesh's shader program
            shader->setUniformF("obj2world", m->obj2world().getRowMajorData());
            shader->setUniformF("world2cam", RenderWorker::getCamera()->world2cam().getRowMajorData());
            shader->setUniformF("cam2ndc", RenderWorker::getCamera()->Cam2NDC().getRowMajorData());
            // no need to bind the ebo again
            // eg: 2 faces => 6 element count
            GLenum err = f->glGetError();
            f->glDrawElements(GL_TRIANGLES, 3 * m->face_count(), GL_UNSIGNED_INT, 0);
            err = f->glGetError();
      }
}