#pragma once

#include <aidl/android/hardware/light/BnLights.h>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

// Default implementation that reports a few placeholder lights.
class Lights : public BnLights {
    ndk::ScopedAStatus setLightState(int id, const HwLightState& state) override;
    ndk::ScopedAStatus getLights(std::vector<HwLight>* lights) override;
};

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
