#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  // Settery wywoływane z __init__.py (to_code)
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  // API wołane z YAML (lambda)
  void set_power(bool on);

  // ESPHome lifecycle
  void setup() override;
  void loop() override;

 protected:
  // Wyjścia PWM
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  // Stan logiczny
  bool power_{false};

  // Wartości kanałów (na razie na sztywno = 1.0)
  float r_{1.0f};
  float g_{1.0f};
  float b_{1.0f};
  float w_{1.0f};

  // Renderer
  void render_();
};

}  // namespace super_rgbw
