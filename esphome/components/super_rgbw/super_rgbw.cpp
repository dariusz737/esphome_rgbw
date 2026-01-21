#include "super_rgbw.h"
#include <algorithm>

namespace super_rgbw {

void SuperRGBW::setup() {
  render_();
}

// ───── POWER ─────

void SuperRGBW::set_power(bool on) {
  power_ = on;
  render_();
}

// ───── KANAŁY ─────

void SuperRGBW::set_r(float v) {
  r_ = clamp(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

void SuperRGBW::set_g(float v) {
  g_ = clamp(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

void SuperRGBW::set_b(float v) {
  b_ = clamp(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

void SuperRGBW::set_w(float v) {
  w_ = clamp(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

// ───── DIM ─────

void SuperRGBW::set_dim(float v) {
  apply_dim_(clamp(v, 0.0f, 1.0f));
  render_();
}

// ───── LOGIKA ─────

void SuperRGBW::update_dim_from_channels_() {
  if (dim_sync_lock_) return;

  dim_sync_lock_ = true;
  dim_ = std::max({r_, g_, b_, w_});
  dim_sync_lock_ = false;
}

void SuperRGBW::apply_dim_(float target_dim) {
  if (dim_sync_lock_) return;

  dim_sync_lock_ = true;

  float max_v = std::max({r_, g_, b_, w_});
  if (max_v <= 0.0f) {
    dim_sync_lock_ = false;
    return;
  }

  float scale = target_dim / max_v;

  if (r_ > 0.0f) r_ = clamp(r_ * scale, 0.0f, 1.0f);
  if (g_ > 0.0f) g_ = clamp(g_ * scale, 0.0f, 1.0f);
  if (b_ > 0.0f) b_ = clamp(b_ * scale, 0.0f, 1.0f);
  if (w_ > 0.0f) w_ = clamp(w_ * scale, 0.0f, 1.0f);

  dim_ = target_dim;

  dim_sync_lock_ = false;
}

// ───── RENDER ─────

void SuperRGBW::render_() {
  if (!out_r_ || !out_g_ || !out_b_ || !out_w_) return;

  if (!power_) {
    out_r_->set_level(0);
    out_g_->set_level(0);
    out_b_->set_level(0);
    out_w_->set_level(0);
    return;
  }

  out_r_->set_level(r_);
  out_g_->set_level(g_);
  out_b_->set_level(b_);
  out_w_->set_level(w_);
}

}  // namespace super_rgbw
