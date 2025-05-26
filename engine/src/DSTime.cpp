#include "DSTime.h"
#include <algorithm>

namespace DSEngine {
    // Initialize static members
    DSTime::TimePoint DSTime::s_StartTime;
    DSTime::TimePoint DSTime::s_LastFrameTime;
    DSTime::TimePoint DSTime::s_CurrentFrameTime;
    float DSTime::s_DeltaTime = 0.0f;
    float DSTime::s_SmoothDeltaTime = 0.0f;
    uint64_t DSTime::s_FrameCount = 0;
    bool DSTime::s_Paused = false;
    float DSTime::s_FPSSamples[FPS_WINDOW_SIZE] = {0};
    int DSTime::s_CurrentFPSSample = 0;

    void DSTime::Init() {
        s_StartTime = Clock::now();
        s_LastFrameTime = s_StartTime;
        s_CurrentFrameTime = s_StartTime;
        std::fill_n(s_FPSSamples, FPS_WINDOW_SIZE, 1.0f / 60.0f);
    }

    void DSTime::Update() {
        if (s_Paused) {
            s_DeltaTime = 0.0f;
            return;
        }

        s_LastFrameTime = s_CurrentFrameTime;
        s_CurrentFrameTime = Clock::now();

        // Calculate raw delta time
        Duration delta = s_CurrentFrameTime - s_LastFrameTime;
        s_DeltaTime = delta.count();

        // Clamp delta time to avoid spikes
        s_DeltaTime = std::min(s_DeltaTime, 0.1f); // Max 100ms frame time

        // Update smooth delta time (weighted average)
        const float smoothFactor = 0.2f;
        s_SmoothDeltaTime = s_SmoothDeltaTime * (1.0f - smoothFactor) + s_DeltaTime * smoothFactor;

        // Update FPS calculation
        s_FPSSamples[s_CurrentFPSSample] = s_DeltaTime;
        s_CurrentFPSSample = (s_CurrentFPSSample + 1) % FPS_WINDOW_SIZE;

        s_FrameCount++;
    }

    float DSTime::GetTime() {
        Duration timeSinceStart = s_CurrentFrameTime - s_StartTime;
        return timeSinceStart.count();
    }

    float DSTime::GetDeltaTime() {
        return s_Paused ? 0.0f : s_DeltaTime;
    }

    float DSTime::GetSmoothDeltaTime() {
        return s_Paused ? 0.0f : s_SmoothDeltaTime;
    }

    uint64_t DSTime::GetFrameCount() {
        return s_FrameCount;
    }

    float DSTime::GetFPS() {
        if (s_Paused) return 0.0f;

        float totalTime = 0.0f;
        for (int i = 0; i < FPS_WINDOW_SIZE; i++) {
            totalTime += s_FPSSamples[i];
        }
        float avgDelta = totalTime / FPS_WINDOW_SIZE;
        return 1.0f / avgDelta;
    }

    void DSTime::SetPaused(bool paused) {
        s_Paused = paused;
    }

    bool DSTime::IsPaused() {
        return s_Paused;
    }
}