#pragma once
#include "core/geometry.h"
class Filter {
public:
      Point2f range;
      virtual Float Evaluate(Float x, Float y) = 0;
      Filter(const Point2f& range) : range(range) {}
};

class BoxFilter : public Filter {
public:
      BoxFilter(const Point2f range = { 0.5f,0.5f }) : range(range) {}
      virtual Float Evaluate(Float x, Float y) override {
            return 1.f;
      }
};