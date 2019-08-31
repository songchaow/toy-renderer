#include "core/ResourceManager.h"
#include "shape/triangle.h"

static ResourceManager _resourceManager;

const QStringList ResourceManager::filters = { "Model Files (*.obj)" };

ResourceManager* ResourceManager::getInstance() {
      return &_resourceManager;
}

void ResourceManager::loadFile(QString path) {
      if (path.endsWith("obj", Qt::CaseInsensitive)) {
            Primitive* p = CreatePrimitiveFromModelFile(path.toStdString());
            _loadList.push_back(p);
      }
}

static std::map<std::string, Shader> shaderStore;

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path) {
      std::string id = vertex_path + fragment_path;
      if (shaderStore.find(id) != shaderStore.end())
            return &shaderStore[id];
      else {
            Shader s(vertex_path, fragment_path);
            if (s.loaded()) {
                  shaderStore[id] = s;
                  return &shaderStore[id];
            }
            else return nullptr;
      }
}