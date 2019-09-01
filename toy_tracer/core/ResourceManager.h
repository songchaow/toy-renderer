#pragma once
#include "core/scene.h"
#include "core/shader.h"
#include <vector>

class ResourceManager {
      Scene _scene;
      std::vector<RendererObject*> _loadList;
public:
      static const QStringList filters;
      static ResourceManager* getInstance();
      std::vector<RendererObject*>& getResourceList() { return _loadList; }
      void loadFile(QString path);
};

Shader* LoadShader(const std::string& vertex_path, const std::string& fragment_path, QOpenGLExtraFunctions* f);