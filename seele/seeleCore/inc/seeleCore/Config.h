#pragma once

#include <cmath>

namespace hidonash
{
    namespace config
    {
        namespace parameters
        {
            constexpr auto maxPitchFactor = 2.f;
            constexpr auto minPitchFactor = 0.4f;
            constexpr auto defaultPitchFactor = 1.f;
            constexpr auto sanctityPrefix = "sanctity_";

            constexpr auto summonStatePrefix = "summonState_";

            constexpr auto distancePrefix = "distance_";
            constexpr auto maxDistanceInSeconds = 2.0f;
            constexpr auto minDistanceInSeconds = 0.0f;
            constexpr auto defaultDistanceInSeconds = 0.0f;

            constexpr auto levelPrefix = "level_";
            constexpr auto maxLevelDb = 6.0f;
            constexpr auto minLevelDb = -30.0f;
            constexpr auto defaultLevelDb = 0.0f;
        }

        namespace constants
        {
            constexpr auto fftFrameSize = 2048;
            constexpr auto analysisSize = 2 * fftFrameSize;
            constexpr auto oversamplingFactor = 16;
            constexpr auto stepSize = fftFrameSize / config::constants::oversamplingFactor;
            constexpr auto expectedPhaseDifference = 2. * M_PI * (double) stepSize / (double) fftFrameSize;
            constexpr auto numMembers = 5;
        }
    }
}
