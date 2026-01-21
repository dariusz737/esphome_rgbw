#pragma once
#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  void setup() override;
  void loop() override;

  // power
  void set_power(bool on);

  // channels
  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);

  // dim
  void set_dim(float v);
  void dim_toggle();
  void dim_stop();

  // config
  void set_fade_time(uint32_t ms);

  // outputs
  void set_out_r(esphome::output::FloatOutput *o) { out_r_ = o; }
  void set_out_g(esphome::output::FloatOutput *o) { out_g_ = o; }
  void set_out_b(esphome::output::FloatOutput *o) { out_b_ = o; }
  void set_out_w(esphome::output::FloatOutput *o) { out_w_ = o; }

 protected:
  // helpers
  void loop_fade_();
  void loop_dim_();
  void render_();
  void update_dim_from_channels_();
  void apply_dim_(float target_dim);

  // outputs
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  // state
  bool power_{false};

  float r_{0}, g_{0}, b_{0}, w_{0};
  float dim_{0.05f};

  bool dim_running_{false};
  bool dim_dir_up_{true};
  bool dim_sync_lock_{false};

  // fade
  float fade_level_{0.0f};
  float fade_start_{0.0f};
  float fade_target_{0.0f};
  uint32_t fade_start_ms_{0};
  uint32_t fade_time_ms_{1000};
  bool fading_off_{false};
};

}  // namespace super_rgbw
