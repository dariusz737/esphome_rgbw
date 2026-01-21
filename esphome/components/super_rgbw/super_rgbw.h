#pragma once
#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  void set_power(bool on);
  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);
  void set_dim(float v);

  void setup() override;
  void loop() override {}

 protected:
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  bool power_{false};
  float dim_{1.0f};
  float r_{0}, g_{0}, b_{0}, w_{0};

  void render_();
};

}  // namespace super_rgbw
