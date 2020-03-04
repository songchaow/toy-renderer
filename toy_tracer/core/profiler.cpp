#include "core/profiler.h"

void Profiler::AddTimeStamp() {
      //times.push_back(std::chrono::system_clock::now());
      const char currReadBuffer = (currRecordBuffer + 1) % 2;
      auto timeNow = std::chrono::system_clock::now();
      auto& last_times = times[currReadBuffer];
      auto& this_times = times[currRecordBuffer];
      if (currIndex == times[currRecordBuffer].size() - 1)
            // the last timestamp, after calling swapBuffers, compare with the previous one
            frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(timeNow - last_times[currIndex]);
      this_times[currIndex] = timeNow;
      if (currIndex > 0) {
            std::chrono::microseconds currInterval =
                  std::chrono::duration_cast<std::chrono::microseconds>(timeNow - this_times[currIndex - 1]);
            // check if the interval is bigger than the max
            if (currInterval > worst_intervals[currIndex - 1]) {
                  worst_intervals[currIndex - 1] = currInterval;
                  worst_interval_record_times[currIndex - 1] = timeNow;
            }
            else {
                  // if the max interval is very old, clear
                  auto now2maxtime = std::chrono::duration_cast<std::chrono::seconds>(timeNow - worst_interval_record_times[currIndex - 1]);
                  if (now2maxtime.count() >= 5) {
                        worst_intervals[currIndex - 1] = std::chrono::seconds::zero();
                        worst_interval_record_times[currIndex - 1] = timeNow;
                  }
            }
      }
      currIndex++;
}

void Profiler::setPhaseNames(const std::vector<std::string>& names) {
      phaseNames = names;
      for(int i=0;i<numBuffer;i++)
            times[i].resize(names.size() + 1);
      worst_intervals.resize(names.size());
      worst_interval_record_times.resize(names.size());
}

void Profiler::PrintProfile() {
      /*std::chrono::microseconds elapsed_seconds =
            std::chrono::duration_cast<std::chrono::microseconds>(times.back() - times.front());*/
      const char currReadBuffer = (currRecordBuffer + 1) % 2;
      auto& last_times = times[currReadBuffer];
      std::string duration_str = "Total Frame: " + std::to_string((Float)frame_duration.count() / 1000.f) + "ms";
      for (int i = 0; i < last_times.size() - 1; i++) {
            std::chrono::microseconds period =
                  std::chrono::duration_cast<std::chrono::microseconds>(last_times[i + 1] - last_times[i]);
            duration_str += '\n';
            duration_str += phaseNames[i] + ": " + std::to_string((Float)period.count() / 1000.f) + "ms";
      }
      renderTextAtTopLeft(duration_str, 1.0);
}

void Profiler::PrintWorstProfile() {
      Float sum = 0;
      for (int i = 0; i < worst_intervals.size(); i++)
            sum += (Float)worst_intervals[i].count() / 1000.f;

      std::string duration_str = "Worst Total: " + std::to_string(sum) + "ms" + '\n';
      //duration_str += "Total Frame: " + std::to_string()
      for (int i = 0; i < phaseNames.size(); i++) {
            duration_str += phaseNames[i] + ": " + std::to_string((Float)worst_intervals[i].count() / 1000.f) + "ms";
            duration_str += '\n';
      }
      renderTextAtTopRight(duration_str, 1.0);
}