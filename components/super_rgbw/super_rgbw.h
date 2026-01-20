#pragma once
#include "esphome.h"

namespace super_rgbw {

class SuperRGBW : public Component {
 public:
  void set_power(bool on);
  void set_dim(float dim);
  void set_rgbw(float r, float g, float b, float w);

  void setup() override;
  void loop() override;

  output::FloatOutput *out_r{nullptr};
  output::FloatOutput *out_g{nullptr};
  output::FloatOutput *out_b{nullptr};
  output::FloatOutput *out_w{nullptr};

 private:
  bool power_{false};
  float dim_{1.0f};
  float r_{0}, g_{0}, b_{0}, w_{0};

  void render_();
};

}  // namespace super_rgbw

