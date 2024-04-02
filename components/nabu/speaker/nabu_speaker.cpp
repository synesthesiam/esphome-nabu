#include "nabu_speaker.h"

#ifdef USE_ESP32

#include <driver/i2s.h>

#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nabu {

static const size_t BUFFER_COUNT = 20;

static const char *const TAG = "nabu.speaker";

void NabuSpeaker::setup() {
  ESP_LOGCONFIG(TAG, "Setting up I2S Audio Speaker...");

  this->buffer_queue_ = xQueueCreate(BUFFER_COUNT, sizeof(DataEvent));
  this->event_queue_ = xQueueCreate(BUFFER_COUNT, sizeof(TaskEvent));
}

void NabuSpeaker::start() { this->state_ = speaker::STATE_STARTING; }
void NabuSpeaker::start_() {
  this->parent_->start_i2s();
  this->state_ = speaker::STATE_RUNNING;

  xTaskCreate(NabuSpeaker::player_task, "speaker_task", 8192, (void *)this, 0,
              &this->player_task_handle_);
}

void NabuSpeaker::player_task(void *params) {
  NabuSpeaker *this_speaker = (NabuSpeaker *)params;

  TaskEvent event;
  event.type = TaskEventType::STARTING;
  xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);

  DataEvent data_event;

  event.type = TaskEventType::STARTED;
  xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);

  int16_t buffer[BUFFER_SIZE / 2];

  while (true) {
    if (xQueueReceive(this_speaker->buffer_queue_, &data_event,
                      100 / portTICK_PERIOD_MS) != pdTRUE) {
      break; // End of audio from main thread
    }
    if (data_event.stop) {
      // Stop signal from main thread
      xQueueReset(this_speaker->buffer_queue_); // Flush queue
      break;
    }
    size_t bytes_written;

    memmove(buffer, data_event.data, data_event.len);
    size_t remaining = data_event.len / 2;
    size_t current = 0;

    while (remaining > 0) {
      uint32_t sample = (buffer[current] << 16) | (buffer[current] & 0xFFFF);
      uint32_t samples[2] = {sample, sample};

      esp_err_t err =
          i2s_write(this_speaker->parent_->get_port(), &samples,
                    sizeof(samples), &bytes_written, (10 / portTICK_PERIOD_MS));
      if (err != ESP_OK) {
        event = {.type = TaskEventType::WARNING, .err = err};
        xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);
        continue;
      }
      remaining--;
      current++;
    }

    event.type = TaskEventType::PLAYING;
    xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);
  }

  i2s_zero_dma_buffer(this_speaker->parent_->get_port());

  event.type = TaskEventType::STOPPING;
  xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);

  event.type = TaskEventType::STOPPED;
  xQueueSend(this_speaker->event_queue_, &event, portMAX_DELAY);

  while (true) {
    delay(10);
  }
}

void NabuSpeaker::stop() {
  if (this->state_ == speaker::STATE_STOPPED)
    return;
  if (this->state_ == speaker::STATE_STARTING) {
    this->state_ = speaker::STATE_STOPPED;
    return;
  }
  this->state_ = speaker::STATE_STOPPING;
  DataEvent data;
  data.stop = true;
  xQueueSendToFront(this->buffer_queue_, &data, portMAX_DELAY);
}

void NabuSpeaker::watch_() {
  TaskEvent event;
  if (xQueueReceive(this->event_queue_, &event, 0) == pdTRUE) {
    switch (event.type) {
    case TaskEventType::STARTING:
      ESP_LOGD(TAG, "Starting I2S Audio Speaker");
      break;
    case TaskEventType::STARTED:
      ESP_LOGD(TAG, "Started I2S Audio Speaker");
      break;
    case TaskEventType::STOPPING:
      ESP_LOGD(TAG, "Stopping I2S Audio Speaker");
      break;
    case TaskEventType::PLAYING:
      this->status_clear_warning();
      break;
    case TaskEventType::STOPPED:
      this->state_ = speaker::STATE_STOPPED;
      vTaskDelete(this->player_task_handle_);
      this->player_task_handle_ = nullptr;
      xQueueReset(this->buffer_queue_);
      ESP_LOGD(TAG, "Stopped I2S Audio Speaker");
      break;
    case TaskEventType::WARNING:
      ESP_LOGW(TAG, "Error writing to I2S: %s", esp_err_to_name(event.err));
      this->status_set_warning();
      break;
    }
  }
}

void NabuSpeaker::loop() {
  switch (this->state_) {
  case speaker::STATE_STARTING:
    this->start_();
    break;
  case speaker::STATE_RUNNING:
  case speaker::STATE_STOPPING:
    this->watch_();
    break;
  case speaker::STATE_STOPPED:
    break;
  }
}

size_t NabuSpeaker::play(const uint8_t *data, size_t length) {
  if (this->state_ != speaker::STATE_RUNNING &&
      this->state_ != speaker::STATE_STARTING) {
    this->start();
  }
  size_t remaining = length;
  size_t index = 0;
  while (remaining > 0) {
    DataEvent event;
    event.stop = false;
    size_t to_send_length = std::min(remaining, BUFFER_SIZE);
    event.len = to_send_length;
    memcpy(event.data, data + index, to_send_length);
    if (xQueueSend(this->buffer_queue_, &event, 0) != pdTRUE) {
      return index;
    }
    remaining -= to_send_length;
    index += to_send_length;
  }
  return index;
}

bool NabuSpeaker::has_buffered_data() const {
  return uxQueueMessagesWaiting(this->buffer_queue_) > 0;
}

} // namespace nabu
} // namespace esphome

#endif // USE_ESP32
