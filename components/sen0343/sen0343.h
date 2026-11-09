#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace sen0343 {

// DFRobot SEN0343 (LWLP5000) +/-600 Pa differential pressure sensor.
class SEN0343Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_pressure_sensor(sensor::Sensor *s) { this->pressure_sensor_ = s; }
  void set_temperature_sensor(sensor::Sensor *s) { this->temperature_sensor_ = s; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void read_measurement_();

  sensor::Sensor *pressure_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
};

}  // namespace sen0343
}  // namespace esphome
