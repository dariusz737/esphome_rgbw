// build bump 1

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  // --- Settery wołane z __init__.py (to_code) ---
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  // --- API wołane z YAML (lambda) ---
  void set_power(bool on);

  void set_dim(float dim);
  void set_r(float value);
  void set_g(float value);
  void set_b(float value);
  void set_w(float value);

  // --- ESPHome lifecycle ---
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

  // Parametry (logiczne, niezależne od ON/OFF)
  float dim_{1.0f};
  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float w_{0.0f};

  // Renderer fizycznych wyjść
  void render_();
};

}  // namespace super_rgbw
