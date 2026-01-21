#include "super_rgbw.h"
#include "esphome/core/helpers.h"

namespace super_rgbw {

void SuperRGBW::setup() {
  // Przy starcie ustawiamy wyjścia zgodnie z aktualnym stanem logicznym
  this->render_();
}

void SuperRGBW::loop() {
  // Na razie nic – miejsce na efekty / auto-CT w przyszłości
}

// =====================
// API z YAML (lambda)
// =====================

void SuperRGBW::set_power(bool on) {
  this->power_ = on;
  this->render_();
}

void SuperRGBW::set_dim(float dim) {
  this->dim_ = clamp(dim, 0.0f, 1.0f);
  this->render_();
}

void SuperRGBW::set_r(float value) {
  this->r_ = clamp(value, 0.0f, 1.0f);
  this->render_();
}

void SuperRGBW::set_g(float value) {
  this->g_ = clamp(value, 0.0f, 1.0f);
  this->render_();
}

void SuperRGBW::set_b(float value) {
  this->b_ = clamp(value, 0.0f, 1.0f);
  this->render_();
}

void SuperRGBW::set_w(float value) {
  this->w_ = clamp(value, 0.0f, 1.0f);
  this->render_();
}

// =====================
// Renderer fizyczny
// =====================

void SuperRGBW::render_() {
  if (!out_r_ || !out_g_ || !out_b_ || !out_w_) {
    return;
  }

  if (!power_) {
    // OFF = fizycznie 0, ALE stan logiczny zostaje
    out_r_->set_level(0.0f);
    out_g_->set_level(0.0f);
    out_b_->set_level(0.0f);
    out_w_->set_level(0.0f);
    return;
  }

  // ON = wartości logiczne * dim
  out_r_->set_level(r_ * dim_);
  out_g_->set_level(g_ * dim_);
  out_b_->set_level(b_ * dim_);
  out_w_->set_level(w_ * dim_);
}

}  // namespace super_rgbw
