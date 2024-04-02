#include "nabu_microphone.h"

#ifdef USE_ESP32

#include <driver/i2s.h>
#include <string.h>

#include "esphome/core/log.h"

namespace esphome {
namespace nabu {

static const char *const TAG = "nabu.microphone";

void NabuMicrophone::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Nabu Microphone...");
}

void NabuMicrophone::start() {
  if (this->state_ == microphone::STATE_RUNNING) {
    return;
  }
  this->state_ = microphone::STATE_STARTING;
}

void NabuMicrophone::start_() {
  this->parent_->start_i2s();
  this->state_ = microphone::STATE_RUNNING;
}

void NabuMicrophone::stop_() { this->state_ = microphone::STATE_STOPPED; }
void NabuMicrophone::stop() { this->state_ = microphone::STATE_STOPPING; }

void NabuMicrophone::loop() {
  switch (this->state_) {
  case microphone::STATE_STARTING: {
    this->start_();
  } break;
  case microphone::STATE_STOPPING:
    this->stop_();
    break;
  default:
    break;
  }
}

size_t NabuMicrophone::read(int16_t *buf, size_t len) {
  // Read left channel only from 32-bit samples
  size_t num_channels = 2;
  size_t samples_wanted = len / sizeof(int32_t);
  std::vector<int32_t> samples32;
  samples32.resize(samples_wanted * num_channels);
  size_t bytes_to_read = samples_wanted * sizeof(int32_t) * num_channels;

  size_t bytes_read = 0;
  esp_err_t err =
      i2s_read(this->parent_->get_port(), samples32.data(), bytes_to_read,
               &bytes_read, (100 / portTICK_PERIOD_MS));
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "Error reading from I2S microphone: %s",
             esp_err_to_name(err));
    this->status_set_warning();
    return 0;
  }
  if (bytes_read == 0) {
    this->status_set_warning();
    return 0;
  }
  this->status_clear_warning();

  // Convert to 16-bit, taking left channel only
  size_t samples_read = bytes_read / sizeof(int32_t);

  std::vector<int16_t> samples16;
  samples16.resize(samples_read / num_channels);

  size_t i16 = 0;
  for (size_t i32 = 0; i32 < samples_read; i32 += num_channels) {
    int32_t sample = samples32[i32];
    float sample_float = (float)sample / INT32_MAX;
    samples16[i16] = (int16_t)(sample_float * INT16_MAX);
    i16++;
  }

  memcpy(buf, samples16.data(), samples16.size() * sizeof(int16_t));
  return samples16.size() * sizeof(int16_t);
}

} // namespace nabu
} // namespace esphome

#endif // USE_ESP32
