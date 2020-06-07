#pragma once
#include <vector>
#include <functional>
#include "core/primitive.h"
enum ToyTracerKey {
      UP,DOWN,LEFT,RIGHT,
      W, S, A, D, NUM_KEYS
};

class Controller {
public:
      struct ControllerTableItem {
            std::vector<ToyTracerKey> matchingKeys;
            PrimitiveBase* object;
            std::function<void(PrimitiveBase*, Float)> operation;
      };
private:
      std::vector<ControllerTableItem> table;
public:
      static Controller* Instance();
      static bool keyState(ToyTracerKey k);
      void AddItem(ControllerTableItem i) { table.push_back(i); }
      static void setKeyState(ToyTracerKey k, bool v);
      void Tick(Float delta);
};

void addKeyboardMoveControl(PrimitiveBase* p, ToyTracerKey up, ToyTracerKey down, ToyTracerKey left, ToyTracerKey right);