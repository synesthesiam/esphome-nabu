#include "nabu_media_player.h"
#include "esphome/core/log.h"

#ifdef USE_ESP_IDF

namespace esphome {
namespace nabu {

static const char *const TAG = "nabu_media_player";

void NabuMediaPlayer::setup() {
  state = media_player::MEDIA_PLAYER_STATE_IDLE;

  ESP_LOGI(TAG, "Set up nabu media player");

  // std::string current_url_{"http://192.168.68.75:8000/Charly%20Bliss/"
  //                          "Young%20Enough/06%20-%20Young%20Enough.mp3"};
  // std::string current_url_{"http://192.168.68.75:8000/16Khz.mp3"};

  // this->parent_->play_music(current_url_);
}

void NabuMediaPlayer::cleanup_() {
  if (this->client_ != nullptr) {
    esp_http_client_cleanup(this->client_);
    this->client_ = nullptr;
  }
}

void NabuMediaPlayer::set_stream_uri(const std::string &new_uri) {
  this->cleanup_();

  ESP_LOGD(TAG, "Opening http connection: %s", new_uri.c_str());
  // this->current_uri_ = new_uri;
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
  if ((this->client_ != nullptr) && this->is_playing_) {
    if (esp_http_client_is_complete_data_received(this->client_)) {
      this->is_playing_ = false;
      this->cleanup_();
      return;
    }

    uint8_t data[640];
    size_t bytes_read =
        esp_http_client_read(this->client_, (char *)data, sizeof(data));
    if (bytes_read > 0) {
      this->speaker_->play(data, bytes_read);
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
    default:
      break;
      //   case media_player::MEDIA_PLAYER_COMMAND_MUTE:
      //     break;
      //   case media_player::MEDIA_PLAYER_COMMAND_UNMUTE:
      //     break;
      //   case media_player::MEDIA_PLAYER_COMMAND_TOGGLE:
      //     if (state == media_player::MEDIA_PLAYER_STATE_PAUSED) {
      //       this->parent_->resume_music();
      //       state = media_player::MEDIA_PLAYER_STATE_PLAYING;
      //     } else {
      //       this->parent_->pause_music();
      //       state = media_player::MEDIA_PLAYER_STATE_PAUSED;
      //     }
      //     break;
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

void NabuMediaPlayer::mute_() {
  //   muted_ = true;
  //   publish_state();
}

void NabuMediaPlayer::unmute_() {
  //   muted_ = false;
  //   publish_state();
}

void NabuMediaPlayer::set_volume_(float volume, bool publish) {
  //   if (publish) {
  //     publish_state();
  // }
}

} // namespace nabu
} // namespace esphome
#endif
