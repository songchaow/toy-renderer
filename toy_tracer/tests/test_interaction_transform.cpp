#include "core/interaction.h"
#include <iostream>

int main() {
      Interaction i;
      Vector3f wo(0, 1, 0);
      Normal3f n(0.7071, 0.7071, 0);
      i.wo = wo;
      i.n = n;
      Vector3f localWo = i.GetLocalWo();
      std::cout << "localWo:" << localWo.x << " " << localWo.y << " " << localWo.z << std::endl;
      return 0;
}