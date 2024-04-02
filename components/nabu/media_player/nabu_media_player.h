#pragma once

#ifdef USE_ESP_IDF

#include "esphome/components/media_player/media_player.h"
#include "esphome/components/speaker/speaker.h"
#include "esphome/core/component.h"

#include <esp_http_client.h>

#include "../nabu.h"

namespace esphome {
namespace nabu {

class NabuMediaPlayer : public Component,
                        public media_player::MediaPlayer,
                        public Parented<NabuComponent> {
public:
  float get_setup_priority() const override {
    return esphome::setup_priority::LATE;
  }
  void setup() override;
  void loop() override;

  // MediaPlayer implementations
  bool is_muted() const override { return this->muted_; }
  media_player::MediaPlayerTraits get_traits() override;

  //
  void set_stream_uri(const std::string &new_uri);
  void start() {}
  void stop() {}
  void set_speaker(speaker::Speaker *speaker) { this->speaker_ = speaker; }

protected:
  // MediaPlayer implementation
  void control(const media_player::MediaPlayerCall &call) override;

  bool muted_{false};
  bool play_intent_{false};
  optional<std::string> current_uri_{};

  esp_http_client_handle_t client_ = nullptr;
  void cleanup_();

  speaker::Speaker *speaker_{nullptr};
  bool is_playing_ = false;
  uint8_t *speaker_buffer_ = nullptr;
  uint32_t next_buffer_time_ = 0;
};

} // namespace nabu
} // namespace esphome

#endif
