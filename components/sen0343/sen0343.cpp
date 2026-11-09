#include "sen0343.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sen0343 {

static const char *const TAG = "sen0343";

static const uint8_t START_CMD[3] = {0xAA, 0x00, 0x80};
static const uint32_t CONVERSION_MS = 30;
static const float PRESSURE_RANGE_PA = 600.0f;  // +/- 600 Pa
static const float TEMP_MAX_C = 85.0f;
static const float TEMP_MIN_C = -40.0f;

void SEN0343Component::setup() {
  if (this->write(START_CMD, sizeof(START_CMD)) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Sensor did not respond at address 0x%02X", this->address_);
    this->mark_failed();
  }
}

void SEN0343Component::update() {
  if (this->write(START_CMD, sizeof(START_CMD)) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to start measurement");
    this->status_set_warning();
    return;
  }
  this->set_timeout("read", CONVERSION_MS, [this]() { this->read_measurement_(); });
}

void SEN0343Component::read_measurement_() {
  uint8_t data[7];
  if (this->read(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to read measurement");
    this->status_set_warning();
    return;
  }
  this->status_clear_warning();

  // 14-bit pressure in the top bits of bytes 1-2; 16-bit temperature in bytes 4-5.
  uint16_t raw_p = (((uint16_t) data[1] << 8) | data[2]) >> 2;
  uint16_t raw_t = ((uint16_t) data[4] << 8) | data[5];

  float pressure = (raw_p / 16384.0f) * (2 * PRESSURE_RANGE_PA) - PRESSURE_RANGE_PA;
  float temperature = (raw_t / 65536.0f) * (TEMP_MAX_C - TEMP_MIN_C) + TEMP_MIN_C;

  ESP_LOGD(TAG, "Pressure: %.2f Pa, temperature: %.1f °C", pressure, temperature);
  if (this->pressure_sensor_ != nullptr)
    this->pressure_sensor_->publish_state(pressure);
  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);
}

void SEN0343Component::dump_config() {
  ESP_LOGCONFIG(TAG, "SEN0343 differential pressure sensor:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Pressure", this->pressure_sensor_);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
}

}  // namespace sen0343
}  // namespace esphome
