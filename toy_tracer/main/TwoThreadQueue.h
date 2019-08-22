#pragma once
#include <atomic>
#include <vector>

template <typename ElementT>
class TwoThreadQueue {
      constexpr size_t pool_size = 200;
      ElementT _pool[pool_size];
      std::atomic<size_t> read_pos = 0;
      std::atomic<size_t> end_pos = 0;

public:
      // Called by the producer thread
      bool addElement(const ElementT& e);
      // Called by the consumer thread
      bool readAll(std::vector<ElementT>& res);
};
