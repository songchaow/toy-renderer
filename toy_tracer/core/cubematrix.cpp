#include <fstream>
#include <vector>
#include <core/transform.h>
#include <core/primitive.h>
#include <shape/flatcube.h>
constexpr unsigned int edgeLength = 32;

Vector3i differenceIdx;

InstancedPrimitive* createCubeMatrix() {
      // first read 3 integers which representing the difference index
      std::ifstream fIdx("index.txt");
      if (!fIdx.is_open())
            LOG(ERROR) << "index.txt does not exist";
      fIdx >> differenceIdx.x;
      fIdx >> differenceIdx.y;
      fIdx >> differenceIdx.z;
      std::ifstream fposition("matrix.txt");
      if (!fposition.is_open()) {
            LOG(ERROR) << "matrix.txt does not exist";
            return nullptr;
      }
      
      /* then read 32x32x32 elements
         the order:
         x, y, z
         0, 0, 0,
         0, 0, 1,
         0, 0, 2,
         ...
         0, 0, 31,
         0, 1, 0,
         0, 1, 1,
         ...
         0, 31, 31,
         1, 0, 0,
         ...

      */
      std::vector<Matrix4> obj2worlds;
      for (int i = 0; i < edgeLength; i++) {
            bool ended = false;
            for (int j = 0; j < edgeLength; j++) {
                  for (int k = 0; k < edgeLength; k++) {
                        int val;
                        fposition >> val;
                        if (val == 1) {
                              // create a position
                              obj2worlds.push_back(TranslateM(Float(i), Float(j), Float(k)));
                        }
                        // if ended
                        if (!fposition.good()) {
                              LOG(WARNING) << "matrix.txt ends prematurely.";
                              ended = true;
                              break;
                        }
                  }
                  if (ended)
                        break;
            }
            if (ended)
                  break;
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
                              if (differenceIdx == Vector3i(0, 0, 0)) {
                                    differenceIdx = Vector3i(i, j, k);
                              }
                        }
                  }
            }
      }
      return new InstancedPrimitive(new FlatCube(), defaultMaterial, obj2worlds);
}