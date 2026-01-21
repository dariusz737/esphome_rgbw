#pragma once
#include "esphome.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  // API z YAML
  void set_power(bool on);

  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);

  void set_dim(float v);

  // Wyjścia
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  void setup() override;
  void loop() override {}

 protected:
  // Core
  void render_();
  void update_dim_from_channels_();
  void apply_dim_(float target_dim);

  // PWM
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  // Stan
  bool power_{false};
  bool dim_sync_lock_{false};

  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float w_{0.0f};
  float dim_{1.0f};
};

}  // namespace super_rgbw
