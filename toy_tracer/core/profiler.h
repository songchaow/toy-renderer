#pragma once
#include <chrono>
#include <vector>
#include <core/rendertext.h>
class Profiler {
      static const char numBuffer = 2;
      std::vector<std::chrono::time_point<std::chrono::system_clock>> times[numBuffer];
      // the following two vectors are paired
      std::vector<std::chrono::microseconds> worst_intervals;
      std::vector<std::chrono::time_point<std::chrono::system_clock>> worst_interval_record_times;
      std::vector<std::string> phaseNames;
      uint32_t currIndex = 0;
      std::chrono::microseconds frame_duration;
      unsigned char currRecordBuffer;
public:
      Profiler() : currRecordBuffer(0) {}
      void AddTimeStamp();
      void Clear() { currIndex = 0; currRecordBuffer = (currRecordBuffer + 1) % 2; }
      void setPhaseNames(const std::vector<std::string>& names);
      void PrintProfile();
      void PrintWorstProfile();
      Float durationSecond() { return (Float)frame_duration.count() / 1000000.f; }
};