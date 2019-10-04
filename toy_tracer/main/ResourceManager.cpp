#include "main/ResourceManager.h"
#include "shape/triangle.h"
#include "main/uiwrapper.h"

static ResourceManager _resourceManager;

const QStringList ResourceManager::filters = { "Model Files (*.obj)" };

ResourceManager* ResourceManager::getInstance() {
      return &_resourceManager;
}

void ResourceManager::loadFile(QString path) {
      if (path.endsWith("obj", Qt::CaseInsensitive)) {
            // TODO: move to main
            Primitive* p = CreatePrimitiveFromModelFile(path.toStdString());
            Primitive_Ui* p_ui = new Primitive_Ui(p);
            _loadList.emplace_back(p_ui);
      }
}

