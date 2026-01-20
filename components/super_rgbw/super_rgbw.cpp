#include "super_rgbw.h"

namespace super_rgbw {

void SuperRGBW::setup() {
  // przy starcie wyrenderuj aktualny stan
  render_();
}

void SuperRGBW::loop() {
  // na razie pusto
  // (tu w przyszłości: efekty, auto-CT, animacje)
}

void SuperRGBW::set_power(bool on) {
  power_ = on;
  render_();
}

void SuperRGBW::set_dim(float dim) {
  dim_ = clamp(dim, 0.0f, 1.0f);
  render_();
}
void SuperRGBW::set_r(float r) {
  r_ = clamp(r, 0.0f, 1.0f);
  render_();
}

void SuperRGBW::set_g(float g) {
  g_ = clamp(g, 0.0f, 1.0f);
  render_();
}

void SuperRGBW::set_b(float b) {
  b_ = clamp(b, 0.0f, 1.0f);
  render_();
}

void SuperRGBW::set_w(float w) {
  w_ = clamp(w, 0.0f, 1.0f);
  render_();
}


void SuperRGBW::render_() {
  // jeżeli nie ma podpiętych wyjść – nic nie rób
  if (!out_r || !out_g || !out_b || !out_w)
    return;

  // POWER = OFF → fizycznie gaśniemy,
  // ale STAN LOGICZNY ZOSTAJE
  if (!power_) {
    out_r->set_level(0);
    out_g->set_level(0);
    out_b->set_level(0);
    out_w->set_level(0);
    return;
  }

  // POWER = ON → skalujemy przez DIM
  out_r->set_level(r_ * dim_);
  out_g->set_level(g_ * dim_);
  out_b->set_level(b_ * dim_);
  out_w->set_level(w_ * dim_);
}

}  // namespace super_rgbw

