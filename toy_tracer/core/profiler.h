#pragma once
#include <chrono>
#include <vector>
#include <core/rendertext.h>
class Profiler {
      std::vector<std::chrono::time_point<std::chrono::system_clock>> times;
      std::vector<std::string> phaseNames;
public:
      Profiler() = default;
      void AddTimeStamp() { times.push_back(std::chrono::system_clock::now()); }
      void Clear() { times.clear(); }
      void setPhaseNames(const std::vector<std::string>& names) { phaseNames = names; }
      void PrintProfile() {
            std::chrono::microseconds elapsed_seconds =
                  std::chrono::duration_cast<std::chrono::microseconds>(times.back() - times.front());
            std::string duration_str = "Total Frame: " + std::to_string((Float)elapsed_seconds.count()/1000) + "ms";
            for (int i = 0; i < times.size() - 1; i++) {
                  std::chrono::microseconds period =
                        std::chrono::duration_cast<std::chrono::microseconds>(times[i+1] - times[i]);
                  duration_str += '\n';
                  duration_str += phaseNames[i] + ": " + std::to_string((Float)period.count()/1000) + "ms";
            }
            renderTextAtTopLeft(duration_str, 1.0);
      }
};