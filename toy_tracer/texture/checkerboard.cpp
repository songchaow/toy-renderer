#include "texture/checkerboard.h"

RGBSpectrum CheckerBoard::Evaluate(Float u, Float v) {
      int u_idx = u / stripSize;
      int v_idx = v / stripSize;
      if ((u_idx + v_idx) & 0x01)
            return _colorASpectrum;
      else
            return _colorBSpectrum;
}