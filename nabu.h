#pragma once

#ifdef USE_ESP_IDF

#include "esphome/core/component.h"

#include <string>

#include <driver/i2s.h>

// #include <audio_element.h>
// #include <audio_pipeline.h>

namespace esphome {
namespace nabu {

enum class State {
  STARTING,
  IDLE,
  PREPARING_MUSIC,
  MUSIC_PREPARED,
  PLAYING_MUSIC,
};

class NabuComponent : public Component {

public:
  float get_setup_priority() const override {
    return esphome::setup_priority::LATE;
  }
  void setup() override;
  void loop() override;

  i2s_pin_config_t get_pin_config() const {
    return {
        .mck_io_num = this->mclk_pin_,
        .bck_io_num = this->bclk_pin_,
        .ws_io_num = this->lrclk_pin_,
        .data_out_num = this->dout_pin_,
        .data_in_num = this->din_pin_,
    };
  }

  i2s_port_t get_port() const { return this->port_; }
  void set_mclk_pin(int pin) { this->mclk_pin_ = pin; }
  void set_bclk_pin(int pin) { this->bclk_pin_ = pin; }
  void set_lrclk_pin(int pin) { this->lrclk_pin_ = pin; }
  void set_dout_pin(int pin) { this->dout_pin_ = pin; }
  void set_din_pin(int pin) { this->din_pin_ = pin; }

  void play_music(const std::string &uri);
  void pause_music();
  void resume_music();
  void stop_music();

  void start_i2s();
  int read_microphone(char *buf, int len);

protected:
  State state_ = State::STARTING;

  i2s_port_t port_ = I2S_NUM_0;
  int mclk_pin_{I2S_PIN_NO_CHANGE};
  int bclk_pin_{I2S_PIN_NO_CHANGE};
  int lrclk_pin_{I2S_PIN_NO_CHANGE};
  int dout_pin_{I2S_PIN_NO_CHANGE};
  int din_pin_{I2S_PIN_NO_CHANGE};

  bool i2s_started_ = false;
  // audio_event_iface_handle_t pipeline_event_;

  // Pipeline that mixes music and TTS streams
  // audio_pipeline_handle_t pipeline_mix_;
  // audio_element_handle_t downmix_;
  // audio_element_handle_t i2s_writer_;
  // void setup_pipeline_mix_();

  // Pipeline for playing music (media player)
  // audio_pipeline_handle_t pipeline_music_;
  // audio_element_handle_t http_reader_;
  // audio_element_handle_t mp3_decoder_;
  // audio_element_handle_t resample_music_;
  // audio_element_handle_t raw_music_;
  // void setup_pipeline_music_();
  // std::string music_uri_;

  // Pipeline for playing TTS feedback (speaker)
  // audio_pipeline_handle_t pipeline_tts_;
  // audio_element_handle_t raw_tts_;
  // void setup_pipeline_tts_();

  // Pipeline for microphone input
  // audio_pipeline_handle_t pipeline_mic_;
  // audio_element_handle_t raw_mic_;
  // audio_element_handle_t i2s_reader_;
  // void setup_pipeline_mic_();
};

} // namespace nabu
} // namespace esphome

#endif
