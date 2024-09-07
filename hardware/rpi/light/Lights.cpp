#include "Lights.h"
#include <fstream>
#include <android-base/logging.h>

namespace aidl {
namespace android {
namespace hardware {
namespace light {

static const std::string kLedGpio18Path = "/sys/class/leds/led_gpio18/brightness";

ndk::ScopedAStatus Lights::setLightState(int id, const HwLightState& state) {
    LOG(INFO) << "Setting light state for LED GPIO 18";
    std::ofstream led_gpio(kLedGpio18Path);

    if (!led_gpio) {
        LOG(ERROR) << "Failed to open LED GPIO 18";
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    int brightness = (state.color == 0x00000000) ? 0 : 255;
    led_gpio << brightness;
    if (!led_gpio) {
        LOG(ERROR) << "Failed to write to LED GPIO 18";
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Lights::getLights(std::vector<HwLight>* lights) {
    lights->push_back({1, 1}); // Adiciona seu LED como único dispositivo de luz
    LOG(INFO) << "Reporting GPIO 18 LED";
    return ndk::ScopedAStatus::ok();
}

}  // namespace light
}  // namespace hardware
}  // namespace android
}  // namespace aidl
