#include "super_rgbw.h"

namespace super_rgbw {

void SuperRGBW::setup() {
  render_();
}

void SuperRGBW::set_power(bool on) {
  power_ = on;
  render_();
}

void SuperRGBW::set_r(float v) {
  r_ = v;
  render_();
}

void SuperRGBW::set_g(float v) {
  g_ = v;
  render_();
}

void SuperRGBW::set_b(float v) {
  b_ = v;
  render_();
}

void SuperRGBW::set_w(float v) {
  w_ = v;
  render_();
}

void SuperRGBW::set_dim(float v) {
  dim_ = v;
  render_();
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
  out_g_->set_level(g_ * dim_);
  out_b_->set_level(b_ * dim_);
  out_w_->set_level(w_ * dim_);
}

}  // namespace super_rgbw
