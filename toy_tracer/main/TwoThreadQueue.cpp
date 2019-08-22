#include "main/TwoThreadQueue.h"

template<typename ElementT>
bool TwoThreadQueue<ElementT>::addElement(const ElementT & e)
{
      // return false if full
      unsigned int epos = std::atomic_load(&end_pos);
      unsigned int count = (epos + pool_size - std::atomic_load(&read_pos) ) % pool_size;
      if (count == pool_size - 1)
            return false;
      _pool[epos] = e;
      std::atomic_fetch_add(&end_pos, 1);
      std::atomic_store(&end_pos, (epos + 1) % pool_size);
      return true;
}

template<typename ElementT>
bool TwoThreadQueue<ElementT>::readAll(std::vector<ElementT>& res)
{
      unsigned int currReadPos = std::atomic_load(&read_pos);
      unsigned int currEndPos = std::atomic_load(&end_pos);
      if (currEndPos == currReadPos)
            // empty
            return false;
      while ((currReadPos%pool_size) != currEndPos)
            res.push_back(_pool[currReadPos++%pool_size]);
      std::atomic_store(&read_pos, currReadPos%pool_size);
      return true;
}


