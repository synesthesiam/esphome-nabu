#include "nabu.h"

#ifdef USE_ESP_IDF

#include <driver/i2s.h>
#include <string.h>

#include <vector>

// #include <audio_element.h>
// #include <audio_event_iface.h>
// #include <audio_pipeline.h>
// #include <downmix.h>
// #include <driver/i2s.h>
// #include <filter_resample.h>
// #include <http_stream.h>
// #include <i2s_stream.h>
// #include <mp3_decoder.h>
// #include <raw_stream.h>

// #include "i2s_stream_mod.h"
// #include "sdk_ext.h"

// #define INDEX_MUSIC_STREAM 0
// #define INDEX_TTS_STREAM 1
// #define TRANSMIT_TIME 500

#include "esphome/core/log.h"

namespace esphome {
namespace nabu {

static const char *const TAG = "nabu";

void NabuComponent::setup() {
  // audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
  // this->pipeline_event_ = audio_event_iface_init(&evt_cfg);

  // this->setup_pipeline_mix_();
  // this->setup_pipeline_music_();
  // this->setup_pipeline_tts_();
  // this->setup_pipeline_mic_();

  // audio_pipeline_run(this->pipeline_mic_);
}

void NabuComponent::loop() {
  // audio_event_iface_msg_t msg{};
  // audio_event_iface_listen(this->pipeline_event_, &msg, 0);

  // if (this->state_ == State::STARTING) {
  //   ESP_LOGD(TAG, "Starting mix pipeline");
  //   audio_pipeline_run(this->pipeline_mix_);
  // this->state_ = State::IDLE;

  // this->play_music("http://192.168.68.75:8000/Charly%20Bliss/Young%20Enough/"
  //                  "06%20-%20Young%20Enough.mp3");
  // }
  // audio_event_iface_discard(this->pipeline_event_);

  // if ((this->state_ == State::PREPARING_MUSIC) &&
  //     (audio_element_get_state(this->mp3_decoder_) == AEL_STATE_RUNNING)) {
  //   audio_element_info_t music_info{};
  //   audio_element_getinfo(this->mp3_decoder_, &music_info);
  //   ESP_LOGD(TAG, "Received music info");

  //   // Change resample filter to match music source
  //   rsp_filter_change_src_info(this->resample_music_,
  //   music_info.sample_rates,
  //                              music_info.channels, music_info.bits);

  //   this->stop_music();

  //   // Run music pipeline
  //   this->state_ = State::PLAYING_MUSIC;
  //   audio_element_set_uri(this->http_reader_, this->music_uri_.c_str());
  //   audio_pipeline_run(this->pipeline_music_);
  // }
}

// ----------------------------------------------------------------------------

void NabuComponent::start_i2s() {
  if (this->i2s_started_) {
    return;
  }

  ESP_LOGD(TAG, "Starting I2S");
  i2s_driver_config_t i2s_driver_cfg = {
      .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX | I2S_MODE_TX),
      .sample_rate = 16000,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      // .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 128,
      // .dma_buf_count = 4,
      // .dma_buf_len = 256,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      // .tx_desc_auto_clear = true,
      // .fixed_mclk = 0,
      // .fixed_mclk = 24576000,
      // .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
      .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT,
  };
  i2s_driver_install(this->get_port(), &i2s_driver_cfg, 0, nullptr);

  i2s_pin_config_t pin_config = this->get_pin_config();
  i2s_set_pin(this->get_port(), &pin_config);
  this->i2s_started_ = true;
}

void NabuComponent::play_music(const std::string &uri) {
  // this->stop_music();

  ESP_LOGD(TAG, "Playing: %s", uri.c_str());
  // this->music_uri_ = uri;
  // audio_element_set_uri(this->http_reader_, this->music_uri_.c_str());
  // audio_element_set_uri(this->http_reader_, uri.c_str());
  // audio_pipeline_run(this->pipeline_music_);

  // Preroll HTTP reader and MP3 decoder to get music info (see loop)
  // this->state_ = State::PREPARING_MUSIC;

  // if (audio_element_run(this->http_reader_) != ESP_OK) {
  //   ESP_LOGE(TAG, "Starting http reader failed");
  // }
  // if (audio_element_run(this->mp3_decoder_) != ESP_OK) {
  //   ESP_LOGE(TAG, "Starting MP3 decoder failed");
  // }
  // if (audio_element_resume(this->http_reader_, 0, 2000 / portTICK_RATE_MS) !=
  //     ESP_OK) {
  //   ESP_LOGE(TAG, "Resuming http reader failed");
  // }
  // if (audio_element_resume(this->mp3_decoder_, 0, 2000 / portTICK_RATE_MS) !=
  //     ESP_OK) {
  //   ESP_LOGE(TAG, "Resuming MP3 decoder failed");
  // }

  // ESP_LOGD(TAG, "Waiting for music info");
}

void NabuComponent::stop_music() {
  // audio_pipeline_stop(this->pipeline_music_);
  // audio_pipeline_reset_items_state(this->pipeline_music_);
  // audio_pipeline_reset_ringbuffer(this->pipeline_music_);
}

void NabuComponent::pause_music() {
  // audio_pipeline_pause(this->pipeline_music_);
}

void NabuComponent::resume_music() {
  // audio_pipeline_resume(this->pipeline_music_);
}

// ----------------------------------------------------------------------------

// void NabuComponent::setup_pipeline_mix_() {
//   // Create a pipeline that will mix music and TTS streams
//   audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();

//   ESP_LOGD(TAG, "Creating mix pipeline");
//   this->pipeline_mix_ = audio_pipeline_init(&pipeline_cfg);
//   audio_pipeline_set_listener(this->pipeline_mix_, this->pipeline_event_);

//   // downmix - mixes the music and tts streams
//   downmix_cfg_t downmix_cfg = {
//       .downmix_info =
//           {
//               .source_num = 2,
//               .out_ctx = ESP_DOWNMIX_OUT_CTX_LEFT_RIGHT,
//               .mode = ESP_DOWNMIX_WORK_MODE_BYPASS,
//               .output_type = ESP_DOWNMIX_OUTPUT_TYPE_ONE_CHANNEL,
//           },
//       .max_sample = DM_BUF_SIZE,
//       .out_rb_size = DOWNMIX_RINGBUFFER_SIZE,
//       .task_stack = DOWNMIX_TASK_STACK,
//       .task_core = DOWNMIX_TASK_CORE,
//       .task_prio = DOWNMIX_TASK_PRIO,
//       .stack_in_ext = true,
//   };
//   this->downmix_ = downmix_init(&downmix_cfg);
//   downmix_set_input_rb_timeout(this->downmix_, 0, INDEX_MUSIC_STREAM);
//   downmix_set_input_rb_timeout(this->downmix_, 0, INDEX_TTS_STREAM);

//   esp_downmix_input_info_t source_information[2] = {0};
//   esp_downmix_input_info_t source_info_base = {
//       .samplerate = 16000,
//       .channel = 1,
//       .bits_num = 16,
//       /* depress from 0dB to -10dB */
//       .gain = {0, -10},
//       .transit_time = TRANSMIT_TIME,
//   };
//   source_information[0] = source_info_base;

//   esp_downmix_input_info_t source_info_newcome = {
//       .samplerate = 16000,
//       .channel = 1,
//       .bits_num = 16,
//       .gain = {0, 10},
//       .transit_time = TRANSMIT_TIME,
//   };
//   source_information[1] = source_info_newcome;
//   source_info_init(this->downmix_, source_information);

//   // Configure i2s
//   i2s_driver_config_t i2s_config = {
//     .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX | I2S_MODE_RX),
//     .sample_rate = 16000,
//     .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
//     .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
//     .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL2 | ESP_INTR_FLAG_IRAM,
//     .dma_buf_count = 2,
//     .dma_buf_len = 960,
//     .use_apll = false,
//     .tx_desc_auto_clear = true,
//     .fixed_mclk = I2S_PIN_NO_CHANGE,
//     .mclk_multiple = I2S_MCLK_MULTIPLE_384,
//     .bits_per_chan = I2S_BITS_PER_CHAN_32BIT,
// #if SOC_I2S_SUPPORTS_TDM
//     .chan_mask = I2S_CHANNEL_MONO,
//     .total_chan = 0,
//     .left_align = false,
//     .big_edin = false,
//     .bit_order_msb = false,
//     .skip_msk = false,
// #endif
//   };

//   i2s_stream_cfg_t i2s_cfg = {
//       .type = AUDIO_STREAM_WRITER,
//       .i2s_config = i2s_config,
//       .i2s_port = this->get_port(),
//       .use_alc = true,
//       .volume = 0,
//       .out_rb_size = (4 * 960),
//       .task_stack = I2S_STREAM_TASK_STACK,
//       .task_core = I2S_STREAM_TASK_CORE,
//       .task_prio = I2S_STREAM_TASK_PRIO,
//       .stack_in_ext = false,
//       .multi_out_num = 0,
//       .uninstall_drv = true,
//       .need_expand = true,
//       .expand_src_bits = I2S_BITS_PER_SAMPLE_16BIT,
//   };

//   this->i2s_writer_ = i2s_stream_init(&i2s_cfg);

//   i2s_pin_config_t pin_config = this->get_pin_config();
//   i2s_set_pin(this->get_port(), &pin_config);
//   i2s_zero_dma_buffer(this->get_port());

//   // if (i2s_stream_set_clk(
//   //         this->i2s_writer_, 16000,
//   //         ((I2S_BITS_PER_CHAN_32BIT << 16) | I2S_BITS_PER_SAMPLE_32BIT),
//   //         2) != ESP_OK) {
//   //   ESP_LOGE(TAG, "error while setting sample rate and bit depth,");
//   // }

//   audio_pipeline_register(this->pipeline_mix_, this->downmix_, "mix");
//   audio_pipeline_register(this->pipeline_mix_, this->i2s_writer_, "i2s");

//   const char *link_mix[2] = {"mix", "i2s"};
//   audio_pipeline_link(this->pipeline_mix_, &link_mix[0], 2);
// }

// void NabuComponent::setup_pipeline_music_() {
//   audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();

//   ESP_LOGD(TAG, "Creating music pipeline");
//   this->pipeline_music_ = audio_pipeline_init(&pipeline_cfg);
//   audio_pipeline_set_listener(this->pipeline_music_, this->pipeline_event_);

//   // http -> mp3 decoder
//   http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
//   this->http_reader_ = http_stream_init(&http_cfg);

//   mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
//   this->mp3_decoder_ = mp3_decoder_init(&mp3_cfg);

//   rsp_filter_cfg_t resample_music_cfg = {
//       .src_rate = 44100, // Updated dynamically when stream starts
//       .src_ch = 2,
//       .dest_rate = 16000,
//       .dest_bits = 16,
//       .dest_ch = 2,
//       .src_bits = 16,
//       .mode = RESAMPLE_DECODE_MODE,
//       .max_indata_bytes = RSP_FILTER_BUFFER_BYTE,
//       .out_len_bytes = RSP_FILTER_BUFFER_BYTE,
//       .type = ESP_RESAMPLE_TYPE_AUTO,
//       .complexity = 2,
//       .down_ch_idx = 0,
//       .prefer_flag = ESP_RSP_PREFER_TYPE_SPEED,
//       .out_rb_size = RSP_FILTER_RINGBUFFER_SIZE,
//       .task_stack = RSP_FILTER_TASK_STACK,
//       .task_core = RSP_FILTER_TASK_CORE,
//       .task_prio = RSP_FILTER_TASK_PRIO,
//       .stack_in_ext = true,
//   };
//   this->resample_music_ = rsp_filter_init(&resample_music_cfg);

//   raw_stream_cfg_t raw_stream_cfg = {
//       .type = AUDIO_STREAM_WRITER,
//       .out_rb_size = 8 * 1024,
//   };
//   this->raw_music_ = raw_stream_init(&raw_stream_cfg);

//   audio_pipeline_register(this->pipeline_music_, this->http_reader_, "http");
//   audio_pipeline_register(this->pipeline_music_, this->mp3_decoder_, "mp3");
//   audio_pipeline_register(this->pipeline_music_, this->resample_music_,
//                           "resample");
//   audio_pipeline_register(this->pipeline_music_, this->raw_music_, "raw");

//   const char *link_music[4] = {"http", "mp3", "resample", "raw"};
//   audio_pipeline_link(this->pipeline_music_, &link_music[0], 4);

//   ringbuf_handle_t rb_music =
//   audio_element_get_input_ringbuf(this->raw_music_);
//   downmix_set_input_rb(this->downmix_, rb_music, INDEX_MUSIC_STREAM);
// }

// void NabuComponent::setup_pipeline_tts_() {
//   audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();

//   ESP_LOGD(TAG, "Creating tts pipeline");
//   this->pipeline_tts_ = audio_pipeline_init(&pipeline_cfg);
//   audio_pipeline_set_listener(this->pipeline_tts_, this->pipeline_event_);

//   raw_stream_cfg_t raw_stream_cfg = {
//       .type = AUDIO_STREAM_WRITER,
//       .out_rb_size = 8 * 1024,
//   };

//   this->raw_tts_ = raw_stream_init(&raw_stream_cfg);
//   audio_pipeline_register(this->pipeline_tts_, this->raw_tts_, "raw");

//   ringbuf_handle_t rb_tts = audio_element_get_input_ringbuf(this->raw_tts_);
//   downmix_set_input_rb(this->downmix_, rb_tts, INDEX_TTS_STREAM);
// }

// void NabuComponent::setup_pipeline_mic_() {
//   i2s_driver_config_t config = {
//       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//       .sample_rate = 16000,
//       .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
//       .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//       .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//       .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//       .dma_buf_count = 4,
//       .dma_buf_len = 256,
//       .use_apll = false,
//       .tx_desc_auto_clear = false,
//       // .fixed_mclk = 0,
//       // .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
//       .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT,
//   };

//   i2s_driver_install(this->get_port(), &config, 0, nullptr);

//   i2s_pin_config_t pin_config = this->get_pin_config();
//   // pin_config.data_in_num = this->din_pin_;
//   ESP_LOGD(TAG, "%d", pin_config.data_in_num);

//   i2s_set_pin(this->get_port(), &pin_config);

//   audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();

//   ESP_LOGD(TAG, "Creating mic pipeline");
//   this->pipeline_mic_ = audio_pipeline_init(&pipeline_cfg);
//   audio_pipeline_set_listener(this->pipeline_mic_, this->pipeline_event_);

//   i2s_driver_config_t i2s_config = {
//     .mode = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX | I2S_MODE_TX),
//     .sample_rate = 16000,
//     .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//     .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//     .communication_format = I2S_COMM_FORMAT_STAND_I2S,
//     //.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2 | ESP_INTR_FLAG_IRAM,
//     .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
//     .dma_buf_count = 4,
//     .dma_buf_len = 256,
//     .use_apll = false,
//     .tx_desc_auto_clear = true,
//     .fixed_mclk = 0,
//     .mclk_multiple = I2S_MCLK_MULTIPLE_256,
//     .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT,
// #if SOC_I2S_SUPPORTS_TDM
//     .chan_mask = I2S_CHANNEL_MONO,
//     .total_chan = 0,
//     .left_align = false,
//     .big_edin = false,
//     .bit_order_msb = false,
//     .skip_msk = false,
// #endif
//   };

//   // if (this->pdm_) {
//   //   i2s_config.mode = (i2s_mode_t)(i2s_config.mode | I2S_MODE_PDM);
//   // }

//   i2s_stream_cfg_t i2s_cfg = {
//       .type = AUDIO_STREAM_READER,
//       .i2s_config = i2s_config,
//       .i2s_port = this->get_port(),
//       .use_alc = false,
//       .volume = 0,
//       .out_rb_size = (8 * 256),
//       .task_stack = I2S_STREAM_TASK_STACK,
//       .task_core = I2S_STREAM_TASK_CORE,
//       .task_prio = I2S_STREAM_TASK_PRIO,
//       .stack_in_ext = false,
//       .multi_out_num = 0,
//       .uninstall_drv = true,
//       .need_expand = false,
//       .expand_src_bits = I2S_BITS_PER_SAMPLE_16BIT,
//   };

//   this->i2s_reader_ = i2s_stream_init(&i2s_cfg);
//   // this->i2s_reader_->buf_size = 2 * 256;

//   i2s_pin_config_t pin_config = this->get_pin_config();
//   // pin_config.data_in_num = this->din_pin_;
//   i2s_set_pin(this->get_port(), &pin_config);

//   audio_element_set_music_info(this->i2s_reader_, 16000, 1,
//                                I2S_BITS_PER_SAMPLE_16BIT);

//   if (i2s_stream_set_clk(
//           this->i2s_reader_, 16000,
//           ((I2S_BITS_PER_CHAN_16BIT << 16) | I2S_BITS_PER_SAMPLE_16BIT),
//           1) != ESP_OK) {
//     ESP_LOGE(TAG, "error while setting sample rate and bit depth,");
//   }

//   // uint32_t bits_cfg = I2S_BITS_PER_SAMPLE_16BIT;
//   // if (this->bits_per_sample_ == I2S_BITS_PER_SAMPLE_24BIT) {
//   //   bits_cfg = (I2S_BITS_PER_CHAN_32BIT << 16) |
//   I2S_BITS_PER_SAMPLE_24BIT;
//   // } else if ((this->bits_per_sample_ == I2S_BITS_PER_SAMPLE_32BIT)) {
//   //   bits_cfg = I2S_BITS_PER_SAMPLE_32BIT;
//   // }

//   raw_stream_cfg_t raw_stream_cfg = {
//       .type = AUDIO_STREAM_READER,
//       // .out_rb_size = 8 * 1024,
//       .out_rb_size = 0,
//   };

//   this->raw_mic_ = raw_stream_init(&raw_stream_cfg);
//   audio_element_set_input_timeout(this->raw_mic_, 1 / portTICK_RATE_MS);

//   audio_pipeline_register(this->pipeline_mic_, this->i2s_reader_, "i2s");
//   audio_pipeline_register(this->pipeline_mic_, this->raw_mic_, "raw");

//   const char *link_mic[2] = {"i2s", "raw"};
//   audio_pipeline_link(this->pipeline_mic_, &link_mic[0], 2);
// }

} // namespace nabu
} // namespace esphome
#endif
