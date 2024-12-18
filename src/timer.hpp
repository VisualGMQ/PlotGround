#pragma once
#include "pch.hpp"

using TimeType = uint64_t;

class Time {
public:
    constexpr static uint32_t NoFpsLimit = std::numeric_limits<uint32_t>::max();
    
    Time();
    TimeType GetElapse() const;
    uint32_t GetFPS() const;
    uint32_t GetAverageFPS() const;
    void BeginRecordElapse();
    void EndRecordElapse();

    Time(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&&) = delete;
    Time& operator=(const Time&) = delete;
    void WaitForFps();

    void SetFpsLimit(uint32_t fps);

private:
    TimeType elapseTime_ = 0;
    TimeType lastElapseTime_ = 0;
    uint32_t limitFps_ = 120;

    std::array<float, 100> fpsStatistic_;
};