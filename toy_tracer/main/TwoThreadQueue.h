#pragma once
#include <atomic>
#include <vector>

constexpr size_t ttqueue_pool_size = 600;

template <class ElementT>
class TwoThreadQueue {
      
      ElementT _pool[ttqueue_pool_size];
      bool _add_flag[ttqueue_pool_size];
      std::atomic<size_t> read_pos = 0;
      std::atomic<size_t> end_pos = 0;

public:
      // Called by the producer thread
      bool addElement(const ElementT& e) {
            // return false if full
            unsigned int epos = std::atomic_load(&end_pos);
            unsigned int count = (epos + ttqueue_pool_size - std::atomic_load(&read_pos)) % ttqueue_pool_size;
            if (count == ttqueue_pool_size - 1)
                  return false;
            _pool[epos] = e;
            _add_flag[epos] = true;
            std::atomic_fetch_add(&end_pos, 1);
            std::atomic_store(&end_pos, (epos + 1) % ttqueue_pool_size);
            return true;
      }
      bool addDeleteElement(const ElementT& e) {
            // return false if full
            unsigned int epos = std::atomic_load(&end_pos);
            unsigned int count = (epos + ttqueue_pool_size - std::atomic_load(&read_pos)) % ttqueue_pool_size;
            if (count == ttqueue_pool_size - 1)
                  return false;
            _pool[epos] = e;
            _add_flag[epos] = false;
            std::atomic_fetch_add(&end_pos, 1);
            std::atomic_store(&end_pos, (epos + 1) % ttqueue_pool_size);
            return true;
      }
      // Called by the consumer thread
      bool readAll(std::vector<ElementT>& resAdd, std::vector<ElementT>& resDel) {
            unsigned int currReadPos = std::atomic_load(&read_pos);
            unsigned int currEndPos = std::atomic_load(&end_pos);
            if (currEndPos == currReadPos)
                  // empty
                  return false;
            while ((currReadPos%ttqueue_pool_size) != currEndPos) {
                  if (_add_flag[currReadPos])
                        resAdd.push_back(_pool[currReadPos%ttqueue_pool_size]);
                  else
                        resDel.push_back(_pool[currReadPos%ttqueue_pool_size]);
                  currReadPos += 1;
            }
            std::atomic_store(&read_pos, currReadPos%ttqueue_pool_size);
            return true;
      }
      // elements to delete are not treated
      bool readAll(ElementT*& startIdx, ElementT*& endIdx, ElementT*& startdIdx, ElementT*& enddIdx) {
            unsigned int currReadPos = std::atomic_load(&read_pos);
            unsigned int currEndPos = std::atomic_load(&end_pos);
            if (currEndPos == currReadPos)
                  // empty
                  return false;
            startIdx = &_pool[currReadPos%ttqueue_pool_size];
            endIdx = &_pool[currEndPos%ttqueue_pool_size];
            startdIdx = enddIdx = nullptr;
      }
};
