#pragma once

#include "esphome/core/component.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/number/number.h"

namespace super_rgbw {

class SuperRGBW : public esphome::Component {
 public:
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  void attach_r_number(esphome::number::Number *num);
  void attach_dim_number(esphome::number::Number *num);

  void set_power(bool on);

  void setup() override;

 protected:
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  bool power_{false};

  float r_{0.0f};
  float dim_{1.0f};

  void render_();
};

}  // namespace super_rgbw
