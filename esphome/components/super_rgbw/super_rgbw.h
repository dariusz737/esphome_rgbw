#pragma once
#include "esphome.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  void setup() override;
  void loop() override;

  void set_power(bool on);

  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);
  void set_dim(float v);

  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  void set_r_number(esphome::number::Number *n) { r_number_ = n; }
  void set_g_number(esphome::number::Number *n) { g_number_ = n; }
  void set_b_number(esphome::number::Number *n) { b_number_ = n; }
  void set_w_number(esphome::number::Number *n) { w_number_ = n; }
  void set_dim_number(esphome::number::Number *n) { dim_number_ = n; }

  void set_fade_time(uint32_t fade_ms);

  void dim_manual_toggle();
  void dim_manual_stop();

 protected:
  void render_();
  void update_dim_from_channels_();
  void apply_dim_(float target_dim);

  void loop_dim_manual_();

  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  esphome::number::Number *r_number_{nullptr};
  esphome::number::Number *g_number_{nullptr};
  esphome::number::Number *b_number_{nullptr};
  esphome::number::Number *w_number_{nullptr};
  esphome::number::Number *dim_number_{nullptr};

  bool power_{false};
  bool dim_sync_lock_{false};

  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float w_{0.0f};
  float dim_{0.0f};

  // fade
  float fade_start_{0.0f};
  float fade_target_{0.0f};
  float fade_level_{0.0f};
  uint32_t fade_start_ms_{0};
  uint32_t fade_time_ms_{1000};
  bool fading_off_{false};

  bool dim_manual_running_{false};
  bool dim_manual_dir_up_{true};
  bool dim_cycle_finished_{true};
  bool dim_manual_running_{false};
  bool dim_manual_dir_up_{true};
  uint32_t dim_manual_last_ms_{0};
};

}  // namespace super_rgbw
