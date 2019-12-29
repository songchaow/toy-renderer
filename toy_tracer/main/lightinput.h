#include <vector>
#include <fstream>

template<int rowCount>
struct LightSignal {
      int col;
      int row;
      bool elementEnabled[rowCount];
      size_t globalStartIdx() const { return rowCount * rowCount * col + rowCount * row; }
};

template<int rowCount>
std::vector<LightSignal<rowCount>> parseLightSignal(std::ifstream& i) {
      std::vector<LightSignal<rowCount>> sigs;
      while (!i.fail()) {
            sigs.emplace_back();
            i >> sigs.back().col >> sigs.back().row;
            for (int idx = 0; idx < rowCount; idx++) {
                  int boolval;
                  i >> boolval;
                  sigs.back().elementEnabled[idx] = static_cast<bool>(boolval);
            }
      }

      return sigs;
}