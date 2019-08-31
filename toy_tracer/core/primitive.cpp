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
      // assume the material to be a simple material
}