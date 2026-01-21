#include "super_rgbw.h"
#include "esphome/core/helpers.h"

namespace super_rgbw {

void SuperRGBW::setup() {
  this->render_();
}

void SuperRGBW::set_power(bool on) {
  power_ = on;
  render_();
}

void SuperRGBW::attach_r_number(esphome::number::Number *num) {
  num->add_on_state_callback([this](float value) {
    r_ = clamp(value, 0.0f, 1.0f);
    render_();
  });
}

void SuperRGBW::attach_dim_number(esphome::number::Number *num) {
  num->add_on_state_callback([this](float value) {
    dim_ = clamp(value, 0.0f, 1.0f);
    render_();
  });
}

void SuperRGBW::render_() {
  if (!out_r_ || !out_g_ || !out_b_ || !out_w_) return;

  if (!power_) {
    out_r_->set_level(0);
    out_g_->set_level(0);
    out_b_->set_level(0);
    out_w_->set_level(0);
    return;
  }

  out_r_->set_level(r_ * dim_);
  out_g_->set_level(0);
  out_b_->set_level(0);
  out_w_->set_level(0);
}

}  // namespace super_rgbw
