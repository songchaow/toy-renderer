#include <engine/controller.h>
#include <algorithm>

static bool _keyState[ToyTracerKey::NUM_KEYS];

static Controller _instance;

Controller* Controller::Instance() {
      return &_instance;
}

bool Controller::keyState(ToyTracerKey k) {
      return _keyState[k];
}
void Controller::setKeyState(ToyTracerKey k, bool v) {
      _keyState[k] = v;
}

void Controller::Tick(Float delta) {
      for (auto& i : table) {
            bool match = false;
            for (ToyTracerKey k : i.matchingKeys)
                  if (_keyState[k]) {
                        match = true;
                        break;
                  }
            if (match) {
                  i.operation(i.object, delta);
            }
      }
}

void Controller::ClearKey(ToyTracerKey k) {
      for(auto itemIt = table.begin(); itemIt != table.end();) {
            auto it = std::find(itemIt->matchingKeys.begin(), itemIt->matchingKeys.end(), k);
            if(it != itemIt->matchingKeys.end())
                  itemIt = table.erase(itemIt);
            else
                  itemIt++;
      }
}

void addKeyboardMoveControl(PrimitiveBase* p, ToyTracerKey up, ToyTracerKey down, ToyTracerKey left, ToyTracerKey right) {
      _instance.ClearKey(up);
      _instance.ClearKey(down);
      _instance.ClearKey(left);
      _instance.ClearKey(right);
      _instance.AddItem({ {up},p,&PrimitiveBase::moveForwardTick });
      _instance.AddItem({ {down},p,&PrimitiveBase::moveBackwardTick });
      _instance.AddItem({ {left},p,&PrimitiveBase::moveLeftTick });
      _instance.AddItem({ {right},p,&PrimitiveBase::moveRightTick });
}