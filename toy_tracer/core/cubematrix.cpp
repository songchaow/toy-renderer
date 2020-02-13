#include <fstream>
#include <vector>
#include <core/transform.h>
#include <core/primitive.h>
#include <shape/flatcube.h>
constexpr unsigned int edgeLength = 32;
InstancedPrimitive* createCubeMatrix() {
      std::ifstream fposition("matrix.txt");
      std::vector<Matrix4> obj2worlds;
      for (int i = 0; i < edgeLength; i++) {
            for (int j = 0; j < edgeLength; j++) {
                  for (int k = 0; k < edgeLength; k++) {
                        int val;
                        fposition >> val;
                        if (val == 1) {
                              // create a position
                              obj2worlds.push_back(TranslateM(Float(i), Float(j), Float(k)));
                        }
                  }
            }
      }
      return new InstancedPrimitive(new FlatCube(), defaultMaterial, obj2worlds);
} 

InstancedPrimitive* createRandomCubeMatrix() {
      std::ifstream fposition("matrix.txt");
      std::vector<Matrix4> obj2worlds;
      for (int i = 0; i < edgeLength; i++) {
            for (int j = 0; j < edgeLength; j++) {
                  for (int k = 0; k < edgeLength; k++) {
                        
                        if (std::rand() % 10 == 0) {
                              // create a position
                              obj2worlds.push_back(TranslateM(Float(i), Float(j), Float(k)));
                        }
                  }
            }
      }
      return new InstancedPrimitive(new FlatCube(), defaultMaterial, obj2worlds);
}