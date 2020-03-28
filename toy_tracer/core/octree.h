#pragma once
#include "core/primitive.h"

struct MeshInstance {
      Primitive* p;
      PBRMaterial* material;
      TriangleMesh* mesh;
};

inline AABB getAABB(const MeshInstance& mInstance) { return mInstance.mesh->aabb(); }


template<typename T>
struct OctreeNode {
      constexpr static uint16_t MAX_ELEMENT_OCTREE_NODE = 5;
      OctreeNode* children[8];
      const T* data[MAX_ELEMENT_OCTREE_NODE];
      uint8_t node_element_size;

      OctreeNode() {
            for (int i = 0; i < 8; i++)
                  children[i] = nullptr;
            for (int i = 0; i < MAX_ELEMENT_OCTREE_NODE; i++)
                  data[i] = nullptr;
            node_element_size = 0;
      }
      bool splited = false;
      bool Full() const { return node_element_size >= MAX_ELEMENT_OCTREE_NODE; }

      
      // add element to this node, assuming totally fitting
      // split if reaches max element count
      // localAABB: transformed to current node(scaled)
      void Add(const T* o, AABB localAABB) {
            if (!splited) {
                  //data.push_back(o);
                  data[node_element_size++] = o;
                  if (Full())
                        Split();
            }
            else // splited
            {

            }
                  
      }
      void Split() {

      }
};

template<typename T>
class Octree {
      AABB aabb;
      constexpr static Float ScaleLen = 2.0;
public:
      
private:
      OctreeNode<T> root;
public:
      // construct
      void setExtend(const std::vector<T*>& objs) {
            for (auto& o : objs) {
                  aabb += getAABB(*o);
            }
            // fix aabb to a cube
            Vector3f ex = aabb.extent();
            Float maxLength = std::max(ex.x, std::max(ex.y, ex.z));
            aabb.pMax = aabb.pMin + Vector3f(maxLength);
      }
      void Add(const T* o) {
            OctreeNode<T>* currNode = &root;
            if (currNode->splited)
                  ;
            else {

            }
      }
      /*    x: 0, 2^0
      z: 2, 2^2
      */
      inline uint8_t childIndex(const AABB& aabb) {
            constexpr Float relaxedMarginSmall = 0.5 - (ScaleLen*0.25 - 0.25);
            constexpr Float relaxedMarginLarge = 0.5 + (ScaleLen*0.25 - 0.25);
            bool pMaxinRegion[8];
            bool pMininRegion[8];
            
            bool xyzFlags[2][3][2];
            for (int pointIndex = 0; pointIndex < 2; pointIndex++) {
                  const Point3f& p = aabb[pointIndex];
                  for (int i = 0; i < 3; i++) {
                        // loop for x,y,z
                        xyzFlags[pointIndex][i][0] = p[i] < relaxedMarginLarge; // small enough
                        xyzFlags[pointIndex][i][1] = p[i] > relaxedMarginSmall; // large enough
                  }
            }
            uint8_t xIndex;
            if (xyzFlags[0][0][0] && xyzFlags[1][0][0])
                  xIndex = 0;
            else if (xyzFlags[0][0][1] && xyzFlags[1][0][1])
                  xIndex = 1;
            else
                  return 8; // failed
            uint8_t yIndex;
            if (xyzFlags[0][1][0] && xyzFlags[1][1][0])
                  yIndex = 0;
            else if (xyzFlags[0][1][1] && xyzFlags[1][1][1])
                  yIndex = 1;
            else
                  return 8;
            uint8_t zIndex;
            if (xyzFlags[0][2][0] && xyzFlags[1][2][0])
                  zIndex = 0;
            else if (xyzFlags[0][2][1] && xyzFlags[1][2][1])
                  zIndex = 1;
            else
                  return 8;

      }

};