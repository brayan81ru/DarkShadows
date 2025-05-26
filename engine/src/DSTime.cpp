#include "DSTime.h"
#include <algorithm>
namespace DSEngine {
    // Initialize static members
    DSTime::TimePoint DSTime::s_StartTime;
    DSTime::TimePoint DSTime::s_LastFrameTime;
    DSTime::TimePoint DSTime::s_CurrentFrameTime;
    float DSTime::s_DeltaTime = 0.0f;
    float DSTime::s_DeltaTimeMS = 0.0f;
    float DSTime::s_SmoothDeltaTime = 0.0f;
    float DSTime::s_SmoothDeltaTimeMS = 0.0f;
    uint64_t DSTime::s_FrameCount = 0;
    bool DSTime::s_Paused = false;
    std::array<float, DSTime::FRAME_TIME_WINDOW> DSTime::s_FrameTimeSamples;
    int DSTime::s_CurrentSampleIndex = 0;
    float DSTime::s_SmoothedFrameTimeMS = 16.666f; // Initialize to ~60FPS

    void DSTime::Init() {
        s_StartTime = Clock::now();
        s_LastFrameTime = s_StartTime;
        s_CurrentFrameTime = s_StartTime;
        std::fill(s_FrameTimeSamples.begin(), s_FrameTimeSamples.end(), 16.666f);
    }

    void DSTime::Update() {
        if (s_Paused) {
            s_DeltaTime = 0.0f;
            s_DeltaTimeMS = 0.0f;
            return;
        }

        s_LastFrameTime = s_CurrentFrameTime;
        s_CurrentFrameTime = Clock::now();

        // Calculate raw delta time in milliseconds
        Duration delta = s_CurrentFrameTime - s_LastFrameTime;
        s_DeltaTimeMS = delta.count();
        s_DeltaTime = s_DeltaTimeMS * 0.001f; // Convert to seconds

        // Clamp to avoid extreme values (e.g., during debugging)
        const float MAX_DELTA_MS = 100.0f; // 100ms max frame time
        s_DeltaTimeMS = std::min(s_DeltaTimeMS, MAX_DELTA_MS);
        s_DeltaTime = std::min(s_DeltaTime, MAX_DELTA_MS * 0.001f);

        // Update smooth delta time (weighted average)
        const float smoothFactor = 0.2f;
        s_SmoothDeltaTimeMS = s_SmoothDeltaTimeMS * (1.0f - smoothFactor) + s_DeltaTimeMS * smoothFactor;
        s_SmoothDeltaTime = s_SmoothDeltaTimeMS * 0.001f;

        // Update frame time samples for smoothing
        s_FrameTimeSamples[s_CurrentSampleIndex] = s_DeltaTimeMS;
        s_CurrentSampleIndex = (s_CurrentSampleIndex + 1) % FRAME_TIME_WINDOW;

        // Calculate smoothed frame time (average of last N frames)
        float total = 0.0f;
        for (float sample : s_FrameTimeSamples) {
            total += sample;
        }
        s_SmoothedFrameTimeMS = total / FRAME_TIME_WINDOW;

        s_FrameCount++;
    }

    float DSTime::GetTime() {
        Duration timeSinceStart = s_CurrentFrameTime - s_StartTime;
        return timeSinceStart.count() * 0.001f; // Return in seconds
    }

    float DSTime::GetDeltaTime() { return s_Paused ? 0.0f : s_DeltaTime; }
    float DSTime::GetDeltaTimeMS() { return s_Paused ? 0.0f : s_DeltaTimeMS; }
    float DSTime::GetSmoothDeltaTime() { return s_Paused ? 0.0f : s_SmoothDeltaTime; }
    float DSTime::GetSmoothDeltaTimeMS() { return s_Paused ? 0.0f : s_SmoothDeltaTimeMS; }

    float DSTime::GetFrameTimeMS() {
        return s_Paused ? 0.0f : s_DeltaTimeMS;
    }

    float DSTime::GetSmoothFrameTimeMS() {
        return s_Paused ? 0.0f : s_SmoothedFrameTimeMS;
    }

    float DSTime::GetFPS() {
        if (s_Paused || s_SmoothedFrameTimeMS == 0.0f) return 0.0f;
        return 1000.0f / s_SmoothedFrameTimeMS;
    }

    uint64_t DSTime::GetFrameCount() { return s_FrameCount; }
    void DSTime::SetPaused(bool paused) { s_Paused = paused; }
    bool DSTime::IsPaused() { return s_Paused; }
}