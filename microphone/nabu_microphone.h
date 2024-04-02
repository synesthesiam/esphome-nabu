#pragma once

#ifdef USE_ESP32

#include "../nabu.h"

#include "esphome/components/microphone/microphone.h"
#include "esphome/core/component.h"

namespace esphome {
namespace nabu {

class NabuMicrophone : public microphone::Microphone,
                       public Component,
                       public Parented<NabuComponent> {
public:
  void setup() override;
  void start() override;
  void stop() override;
  void loop() override;

  size_t read(int16_t *buf, size_t len) override;

protected:
  void start_();
  void stop_();
};

} // namespace nabu
} // namespace esphome

#endif // USE_ESP32
