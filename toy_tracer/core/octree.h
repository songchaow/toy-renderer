#pragma once
#include "core/primitive.h"

struct MeshInstance {
      Primitive* p;
      PBRMaterial* material;
      TriangleMesh* mesh;
      mutable AABB localAABB;
};

inline AABB getAABB(const MeshInstance& mInstance) { return mInstance.mesh->aabb(); }
inline AABB transformAABB(const AABB& aabb, uint8_t subBoxIndex) {
      bool xIndex = subBoxIndex & 0b001;
      Float xOrigin = xIndex * 0.5;
      bool yIndex = subBoxIndex & 0b010;
      Float yOrigin = yIndex * 0.5;
      bool zIndex = subBoxIndex & 0b100;
      Float zOrigin = zIndex * 0.5;
      Point3f newPmax(aabb.pMax.x - xOrigin, aabb.pMax.y - yOrigin, aabb.pMax.z - zOrigin);
      newPmax *= 2;
      Point3f newPmin(aabb.pMin.x - xOrigin, aabb.pMin.y - yOrigin, aabb.pMin.z - zOrigin);
      newPmin *= 2;
      return AABB(newPmax, newPmin);
}

template<typename T>
struct LinkList {
      uint8_t _size; // cached
      struct LinkListNode {
            T data;
            LinkListNode* next;
      };
      LinkListNode* head;
      // insert after pos
      void insert(LinkListNode* pos, const T& newT) {
            if (!head) {
                  head = new LinkListNode;
                  head->data = newT;
                  head->next = nullptr;
                  return;
            }
            LinkListNode* posAfterOriginal = pos->next;
            LinkListNode* newNode = new LinkListNode;
            newNode->data = newT;
            newNode->next = posAfterOriginal;
            pos->next = newNode;
            ++_size;
      }
      void insert(LinkListNode* pos, LinkListNode* p) {
            if (!head) {
                  head = p;
                  p->next = nullptr;
                  return;
            }
            LinkListNode* posAfterOriginal = pos->next;
            p->next = posAfterOriginal;
            pos->next = p;
            ++_size;
      }
      void insert(LinkListNode* p) {
            insert(head, p);
      }
      void insert(const T& newT) {
            insert(head, newT);
      }
      void Delete(LinkListNode* posPrev) {
            if (!posPrev) {
                  // remove head
                  LinkListNode* newHead = head->next;
                  delete head;
                  head = newHead;
                  return;
            }
            LinkListNode* element = posPrev->next;
            if (!element)
                  return;
            LinkListNode* posAfter = element->next;
            delete element;
            posPrev->next = posAfter;
            --_size;
      }
      // remove but NOT delete
      LinkListNode* Remove(LinkListNode* posPrev) {
            if (!posPrev) {
                  // remove head
                  LinkListNode* newHead = head->next;
                  LinkListNode* ret = head;
                  head = newHead;
                  return ret;
            }
            LinkListNode* element = posPrev->next;
            if (!element)
                  return nullptr;
            LinkListNode* posAfter = element->next;
            posPrev->next = posAfter;
            --_size;
            return element;
      }
      LinkList(const T& headT) : _size(1) {
            head = new LinkListNode;
            head->data = headT;
            head->next = nullptr;
      }
      LinkList() : _size(0) {
            head = nullptr;
      }
      ~LinkList() {
            // cascaded deletion
            while (head) {
                  LinkListNode* pNext = head->next;
                  delete head;
                  head = pNext;
            }
      }
};

template<typename T>
struct OctreeNode {
      constexpr static uint16_t MAX_ELEMENT_OCTREE_NODE = 5;
      OctreeNode* children[8];
      LinkList<T> data;

      OctreeNode() {
            for (int i = 0; i < 8; i++)
                  children[i] = nullptr;
      }
      bool splited = false;
      bool shouldSplit() const { return data._size >= MAX_ELEMENT_OCTREE_NODE; }

      void AddElementNode(typename LinkList<T>::LinkListNode* p) {
            if (!splited) {
                  data.insert(p);
                  if (shouldSplit())
                        Split();
            }
            else {
                  uint8_t childIndex = Octree<T>::childIndex(p->data.localAABB);
                  if (childIndex == 8) {
                        // still at local
                        data.insert(p);
                        return;
                  }
                  if (!children[childIndex])
                        children[childIndex] = new OctreeNode;
                  p->data.localAABB = transformAABB(p->data.localAABB, childIndex);
                  children[childIndex]->AddElementNode(p);
            }
      }
      // add element to this node, assuming totally fitting
      // split if reaches max element count
      // localAABB: transformed to current node(scaled)
      // T o is copied in the end
      void Add(const T& o) {
            const AABB& localAABB = o.localAABB;
            if (!splited) {
                  //data.push_back(o);
                  data.insert(data.head, o);
                  if (shouldSplit())
                        Split();
            }
            else // splitted
            {
                  uint8_t childIndex = Octree<T>::childIndex(localAABB);
                  if (childIndex == 8) {
                        // still at local
                        data.insert(o);
                        return;
                  }
                  if (!children[childIndex])
                        children[childIndex] = new OctreeNode;
                  o.localAABB = transformAABB(localAABB, childIndex);
                  children[childIndex]->Add(o);
            }
                  
      }
      void Split() {
            // check children
            LinkList<T>::LinkListNode* pPrev = nullptr;
            for (auto* p = data.head; p != nullptr;) {
                  uint8_t idx = Octree<T>::childIndex(p->data.localAABB);
                  if (idx != 8) {
                        // transform to child node, and remove in link list
                        if (children[idx] == nullptr)
                              children[idx] = new OctreeNode;
                        p->data.localAABB = transformAABB(p->data.localAABB, idx);
                        // first remove. The move procedure modifies p's next
                        data.Remove(pPrev);
                        children[idx]->AddElementNode(p);
                        p = pPrev->next;
                        // pPrev unmodified!
                  }
                  else {
                        // propagate normally
                        pPrev = p;
                        p = p->next;
                  }
            }
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
            // transform aabb to octree, with octree's pMin as the origin
            Vector3f octreeExtent = aabb.extent();
            Point3f pMax = Point3f(o->mesh->aabb().pMax - aabb.pMin);
            pMax /= octreeExtent;
            Point3f pMin = Point3f(o->mesh->aabb().pMin - aabb.pMin);
            pMin /= octreeExtent;
            // the bbox should be inside octree's bbox
            assert(pMin.x >= 0 && pMin.y >= 0 && pMin.z >= 0
                  && pMax.x <= 1.01 && pMax.y <= 1.01 && pMax.z <= 1.01);
            root.Add(*o);
      }
      /*    Return index of the subbox that fully contains aabb
            AABB already transformed to current node
            x: 0, 2^0; y: 1, 2^1
            z: 2, 2^2
      */
      static inline uint8_t childIndex(const AABB& aabb) {
            constexpr Float relaxedMarginLargeStart = 0.5 - (ScaleLen*0.25 - 0.25);
            constexpr Float relaxedMarginLargeEnd = 1.0 + (ScaleLen*0.25 - 0.25);
            constexpr Float relaxedMarginSmallEnd = 0.5 + (ScaleLen*0.25 - 0.25);
            constexpr Float relaxedMarginSmallStart = 0.0 - (ScaleLen*0.25 - 0.25);
            bool pMaxinRegion[8];
            bool pMininRegion[8];
            // flags to mark whether the 2 points are in each subregion
            bool xyzFlags[2][3][2];
            for (int pointIndex = 0; pointIndex < 2; pointIndex++) {
                  const Point3f& p = aabb[pointIndex];
                  for (int i = 0; i < 3; i++) {
                        // loop for x,y,z
                        xyzFlags[pointIndex][i][0] = p[i] < relaxedMarginSmallEnd && p[i] > relaxedMarginSmallStart; // small enough
                        xyzFlags[pointIndex][i][1] = p[i] > relaxedMarginLargeStart && p[i] < relaxedMarginLargeEnd; // large enough
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
            return xIndex + yIndex << 1 + zIndex << 2;
      }
      static inline uint8_t childIndex(const SphereBound& sb) {
            // 3x4 plane intersect tests
      }

};

// debug use. 
template<typename T>
void glDrawOctree(Octree<T>* tree) {

}