#include "core/octree.h"
#include "shape/triangle.h"

// vertex layout for octree draw
// Point3f: pMin
// Point3f: pMax
// uint8_t: level
Layout octreeVertexLayout = Layout({ DEFAULT_POINT2F_LAYOUT, DEFAULT_UINT8_LAYOUT });

void initializeOctreeDraw() {
      static bool initialized = false;
      if (initialized)
            return;



      initialized = true;
}