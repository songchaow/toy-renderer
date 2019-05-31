#include "core/interaction.h"
#include "core/material.h"
#include <iostream>

int main() {
      Interaction i;
      Vector3f wo(0, 1, 0);
      Normal3f n(0.7071, 0.7071, 0);
      i.wo = wo;
      i.n = n;
      Vector3f localWo = i.GetLocalWo();
      // wo calculated by sample_f
      Dielectric* d = new Dielectric();
      FlatMaterial* flat_m = new FlatMaterial(d);
      Vector3f localWi;
      Spectrum reflect = flat_m->delta_f(localWo, localWi, Vector3f(0, 0, 1), nullptr, true);
      std::cout << "localWo:" << localWo.x << " " << localWo.y << " " << localWo.z << std::endl;
      Vector3f wi = i.GlobalDirection(localWi);
      return 0;
}