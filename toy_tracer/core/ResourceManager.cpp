#include "core/ResourceManager.h"
#include "shape/triangle.h"

static ResourceManager _resourceManager;

const QStringList ResourceManager::filters = { "Model Files (*.obj)" };

ResourceManager* ResourceManager::getInstance() {
      return &_resourceManager;
}

void ResourceManager::loadFile(QString path) {
      if (path.endsWith("obj", Qt::CaseInsensitive)) {
            TriangleMesh* mesh = LoadTriangleMesh(path.toStdString());
            Primitive* p = CreatePrimitiveFromMesh(mesh);
            _loadList.push_back(p);
      }
}

