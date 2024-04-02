#include "nabu_media_player.h"

#ifdef USE_ESP_IDF

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nabu {

static const char *const TAG = "nabu_media_player";
static const size_t BUFFER_MS = 100;
static const size_t BUFFER_SIZE = BUFFER_MS * (16000 / 1000) * sizeof(int16_t);

void NabuMediaPlayer::setup() {
  state = media_player::MEDIA_PLAYER_STATE_IDLE;

  ExternalRAMAllocator<uint8_t> allocator(
      ExternalRAMAllocator<uint8_t>::ALLOW_FAILURE);
  this->speaker_buffer_ = allocator.allocate(BUFFER_SIZE);
  if (this->speaker_buffer_ == nullptr) {
    ESP_LOGW(TAG, "Could not allocate speaker buffer");
    this->mark_failed();
    return;
  }

  ESP_LOGI(TAG, "Set up nabu media player");
}

void NabuMediaPlayer::cleanup_() {
  if (this->client_ != nullptr) {
    esp_http_client_cleanup(this->client_);
    this->client_ = nullptr;
  }

  this->next_buffer_time_ = 0;
  this->is_playing_ = false;
}

void NabuMediaPlayer::set_stream_uri(const std::string &new_uri) {
  this->cleanup_();

  ESP_LOGD(TAG, "Opening http connection: %s", new_uri.c_str());
  esp_http_client_config_t config = {
      .url = new_uri.c_str(),
      .cert_pem = nullptr,
      .disable_auto_redirect = false,
      .max_redirection_count = 10,
  };
  this->client_ = esp_http_client_init(&config);

  esp_err_t err;
  if ((err = esp_http_client_open(this->client_, 0)) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    this->cleanup_();
    return;
  }

  int content_length = esp_http_client_fetch_headers(this->client_);
  ESP_LOGD(TAG, "content_length = %d", content_length);
  if (content_length <= 0) {
    ESP_LOGE(TAG, "Failed to get content length: %s", esp_err_to_name(err));
    this->cleanup_();
    return;
  }

  char wav_header[44];
  esp_http_client_read_response(this->client_, wav_header, sizeof(wav_header));
}

void NabuMediaPlayer::loop() {
  if ((this->next_buffer_time_ > 0) && (this->next_buffer_time_ > millis())) {
    return;
  }

  if ((this->client_ != nullptr) && this->is_playing_) {
    if (esp_http_client_is_complete_data_received(this->client_)) {
      this->is_playing_ = false;
      this->cleanup_();
      return;
    }

    size_t bytes_read = esp_http_client_read(
        this->client_, (char *)this->speaker_buffer_, BUFFER_SIZE);
    if (bytes_read > 0) {
      this->speaker_->play(this->speaker_buffer_, bytes_read);
      this->next_buffer_time_ = millis() + (BUFFER_MS * 0.8);
    }
  }
}

void NabuMediaPlayer::control(const media_player::MediaPlayerCall &call) {
  if (call.get_media_url().has_value()) {
    set_stream_uri(call.get_media_url().value());
    this->is_playing_ = true;
    state = media_player::MEDIA_PLAYER_STATE_PLAYING;
    this->publish_state();
    return;
  }

  // if (call.get_volume().has_value()) {
  //   set_volume_(call.get_volume().value());
  //   unmute_();
  // }

  if (call.get_command().has_value()) {
    switch (call.get_command().value()) {
    case media_player::MEDIA_PLAYER_COMMAND_PLAY:
      if (state == media_player::MEDIA_PLAYER_STATE_PAUSED) {
        this->is_playing_ = true;
        state = media_player::MEDIA_PLAYER_STATE_PLAYING;
        this->publish_state();
      }
      break;
    case media_player::MEDIA_PLAYER_COMMAND_PAUSE:
      this->is_playing_ = false;
      state = media_player::MEDIA_PLAYER_STATE_PAUSED;
      this->publish_state();
      break;
    case media_player::MEDIA_PLAYER_COMMAND_STOP:
      this->is_playing_ = false;
      this->cleanup_();
      state = media_player::MEDIA_PLAYER_STATE_IDLE;
      this->publish_state();
      break;
    case media_player::MEDIA_PLAYER_COMMAND_MUTE:
      this->is_playing_ = false;
      break;
    case media_player::MEDIA_PLAYER_COMMAND_UNMUTE:
      this->is_playing_ = true;
      break;
    default:
      break;
    case media_player::MEDIA_PLAYER_COMMAND_TOGGLE:
      if (state == media_player::MEDIA_PLAYER_STATE_PAUSED) {
        this->is_playing_ = true;
        state = media_player::MEDIA_PLAYER_STATE_PLAYING;
        this->publish_state();
      } else {
        this->is_playing_ = false;
        state = media_player::MEDIA_PLAYER_STATE_PAUSED;
        this->publish_state();
      }
      break;
      //   case media_player::MEDIA_PLAYER_COMMAND_VOLUME_UP: {
      //     //       float new_volume = this->volume + 0.1f;
      //     //       if (new_volume > 1.0f)
      //     //         new_volume = 1.0f;
      //     //       set_volume_(new_volume);
      //     //       unmute_();
      //     break;
      //   }
      //   case media_player::MEDIA_PLAYER_COMMAND_VOLUME_DOWN: {
      //     // float new_volume = this->volume - 0.1f;
      //     // if (new_volume < 0.0f)
      //     //   new_volume = 0.0f;
      //     // set_volume_(new_volume);
      //     // unmute_();
      //     break;
      //   }
    }
  }
}

// pausing is only supported if destroy_pipeline_on_stop is disabled
media_player::MediaPlayerTraits NabuMediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_pause(true);
  return traits;
};

} // namespace nabu
} // namespace esphome
#endif
