#pragma once
#include <chrono>
namespace DSEngine {
    class DSTime {
    public:
        // Initializes the time system
        static void Init();

        // Updates time calculations (call once per frame)
        static void Update();

        // Time since game start in seconds
        static float GetTime();

        // Time since last frame in seconds
        static float GetDeltaTime();

        // Smooth averaged delta time over several frames
        static float GetSmoothDeltaTime();

        // Frame count since game start
        static uint64_t GetFrameCount();

        // Current FPS (smoothed)
        static float GetFPS();

        // Pause/unpause the time system
        static void SetPaused(bool paused);
        static bool IsPaused();

    private:
        // High-resolution clock types
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;
        using Duration = std::chrono::duration<float>;

        // Core timing variables
        static TimePoint s_StartTime;
        static TimePoint s_LastFrameTime;
        static TimePoint s_CurrentFrameTime;
        static float s_DeltaTime;
        static float s_SmoothDeltaTime;
        static uint64_t s_FrameCount;
        static bool s_Paused;

        // For FPS calculation
        static constexpr int FPS_WINDOW_SIZE = 60;
        static float s_FPSSamples[FPS_WINDOW_SIZE];
        static int s_CurrentFPSSample;
    };
}