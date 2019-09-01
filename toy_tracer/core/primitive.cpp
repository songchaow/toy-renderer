#include "core/primitive.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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
      // RTMaterial must be valid now
      assert(rt_m!=nullptr);
      rt_m->load(f);
      shader = LoadShader("shader/vertex.glsl", "pbr_pixel.glsl", f);
}

void Primitive::draw(QOpenGLExtraFunctions* f) {
      // draw all meshes
      // TODO: maybe different meshes' materials/textures are different.
      if (rt_m->albedo_map) {
            f->glActiveTexture(GL_TEXTURE0);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->albedo_map->tbo());
      }
      if (rt_m->metallic_map) {
            f->glActiveTexture(GL_TEXTURE1);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->metallic_map->tbo());
      }
      if (rt_m->rough_map) {
            f->glActiveTexture(GL_TEXTURE2);
            f->glBindTexture(GL_TEXTURE_2D, rt_m->rough_map->tbo());
      }

      
      for (auto& m : _meshes) {
            f->glBindVertexArray(m->vao());
            // no need to bind the ebo again
            

      }
}