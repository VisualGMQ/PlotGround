#include "timer.hpp"
#include "macro.hpp"

Time::Time() {
    fpsStatistic_.fill(0);
}

void Time::BeginRecordElapse() {
    elapseTime_ = SDL_GetTicks64();
}

void Time::EndRecordElapse() {
    elapseTime_ = SDL_GetTicks64() - elapseTime_;
    lastElapseTime_ = elapseTime_;

    for (int i = 0; i < fpsStatistic_.size() - 1; i++) {
        fpsStatistic_[i] = fpsStatistic_[i + 1];
    }
    fpsStatistic_.back() = elapseTime_;
}

void Time::WaitForFps() {
    
    float limitElapseTime = 1000.0f / limitFps_;
    float elapseTime = SDL_GetTicks64() - elapseTime_;
    float waitTime = limitElapseTime - elapseTime;
    SDL_Delay(std::max(waitTime, 0.0f));
}

void Time::SetFpsLimit(uint32_t fps) {
    limitFps_ = fps;
}

TimeType Time::GetElapse() const {
    return lastElapseTime_;
}

uint32_t Time::GetFPS() const {
    if (lastElapseTime_ == 0) {
        return std::numeric_limits<uint32_t>::max();
    }
    return 1000 / lastElapseTime_;
}

uint32_t Time::GetAverageFPS() const {
    float sum = 0;
    for (int i = 0; i < fpsStatistic_.size(); i++) {
        sum += fpsStatistic_[i];
    }

    return uint32_t(1000.0f / (sum / fpsStatistic_.size()));
}