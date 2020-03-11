#include "main/ResourceManager.h"
#include "shape/triangle.h"
#include "main/uiwrapper.h"
#include "core/glTFLoader.h"

static ResourceManager _resourceManager;

const QStringList ResourceManager::filters = { "Model Files (*.gltf)" };

ResourceManager* ResourceManager::getInstance() {
      return &_resourceManager;
}

void ResourceManager::loadFile(QString path) {
      if (path.endsWith("gltf", Qt::CaseInsensitive)) {
            std::vector<Primitive*> ps = LoadGLTF(path.toStdString());
            for (auto& p : ps) {
                  Primitive_Ui* p_ui = new Primitive_Ui(p);
                  _loadList.emplace_back(p_ui);
            }
      }
}

void ResourceManager::addPrimitive(Primitive* p) {
      Primitive_Ui* p_ui = new Primitive_Ui(p);
      _loadList.emplace_back(p_ui);
}