#include "super_rgbw.h"

namespace super_rgbw {

void SuperRGBW::setup() {
  // Na starcie: wszystko zgaszone
  render_();
}

void SuperRGBW::loop() {
  // Na razie NIC nie robimy w pętli
  // (później tu wejdzie auto-CT, FX, timery itd.)
}

void SuperRGBW::set_power(bool on) {
  power_ = on;
  render_();
}

void SuperRGBW::render_() {
  if (out_r_ == nullptr || out_g_ == nullptr ||
      out_b_ == nullptr || out_w_ == nullptr) {
    return;  // zabezpieczenie
  }

  if (!power_) {
    // POWER = OFF → fizycznie gaśniemy
    out_r_->set_level(0.0f);
    out_g_->set_level(0.0f);
    out_b_->set_level(0.0f);
    out_w_->set_level(0.0f);
    return;
  }

  // POWER = ON → świecimy zapamiętanymi wartościami
  out_r_->set_level(r_);
  out_g_->set_level(g_);
  out_b_->set_level(b_);
  out_w_->set_level(w_);
}

}  // namespace super_rgbw
