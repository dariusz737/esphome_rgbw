#include "super_rgbw.h"
#include <algorithm>

namespace super_rgbw {

static constexpr float DIM_FLOOR = 0.05f;

static inline float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

void SuperRGBW::setup() {
  render_();
}

// ───── MAIN LOOP ─────
void SuperRGBW::loop() {
  loop_fade_();
  loop_dim_();
}

// ───── POWER / FADE ─────
void SuperRGBW::set_power(bool on) {
  fade_start_ = fade_level_;
  fade_target_ = on ? 1.0f : 0.0f;
  fade_start_ms_ = millis();

  if (on) {
    power_ = true;
    fading_off_ = false;
  } else {
    fading_off_ = true;
  }
}

void SuperRGBW::loop_fade_() {
  if (fade_level_ == fade_target_) return;

  uint32_t now = millis();
  float t = float(now - fade_start_ms_) / float(fade_time_ms_);

  if (t >= 1.0f) {
    fade_level_ = fade_target_;
    if (fading_off_) {
      power_ = false;
      fading_off_ = false;
    }
    render_();
    return;
  }

  fade_level_ = fade_start_ + (fade_target_ - fade_start_) * t;
  fade_level_ = clampf(fade_level_, 0.0f, 1.0f);
  render_();
}

void SuperRGBW::set_fade_time(uint32_t ms) {
  fade_time_ms_ = std::max<uint32_t>(ms, 1);
}

// ───── CHANNELS ─────
void SuperRGBW::set_r(float v) { r_ = clampf(v, 0, 1); update_dim_from_channels_(); if (power_) render_(); }
void SuperRGBW::set_g(float v) { g_ = clampf(v, 0, 1); update_dim_from_channels_(); if (power_) render_(); }
void SuperRGBW::set_b(float v) { b_ = clampf(v, 0, 1); update_dim_from_channels_(); if (power_) render_(); }
void SuperRGBW::set_w(float v) { w_ = clampf(v, 0, 1); update_dim_from_channels_(); if (power_) render_(); }

// ───── DIM ─────
void SuperRGBW::set_dim(float v) {
  apply_dim_(clampf(v, DIM_FLOOR, 1.0f));
  if (power_) render_();
}

void SuperRGBW::dim_toggle() {
  if (!dim_running_) {
    dim_running_ = true;
  } else {
    dim_dir_up_ = !dim_dir_up_;
  }
}

void SuperRGBW::dim_stop() {
  dim_running_ = false;
}

void SuperRGBW::loop_dim_() {
  if (!dim_running_) return;

  const float STEP = 0.02f;
  float next = dim_ + (dim_dir_up_ ? STEP : -STEP);

  if (next >= 1.0f) {
    next = 1.0f;
    dim_running_ = false;
  }
  if (next <= DIM_FLOOR) {
    next = DIM_FLOOR;
    dim_running_ = false;
  }

  apply_dim_(next);
  if (power_) render_();
}

// ───── DIM LOGIC ─────
void SuperRGBW::update_dim_from_channels_() {
  if (dim_sync_lock_) return;
  dim_sync_lock_ = true;
  dim_ = std::max({r_, g_, b_, w_});
  dim_ = clampf(dim_, DIM_FLOOR, 1.0f);
  dim_sync_lock_ = false;
}

void SuperRGBW::apply_dim_(float target_dim) {
  if (dim_sync_lock_) return;
  dim_sync_lock_ = true;

  float max_v = std::max({r_, g_, b_, w_});
  if (max_v > 0.0f) {
    float scale = target_dim / max_v;
    if (r_ > 0) r_ = clampf(r_ * scale, 0, 1);
    if (g_ > 0) g_ = clampf(g_ * scale, 0, 1);
    if (b_ > 0) b_ = clampf(b_ * scale, 0, 1);
    if (w_ > 0) w_ = clampf(w_ * scale, 0, 1);
  }

  dim_ = clampf(target_dim, DIM_FLOOR, 1.0f);
  dim_sync_lock_ = false;
}

// ───── RENDER ─────
void SuperRGBW::render_() {
  if (!power_) {
    out_r_->set_level(0);
    out_g_->set_level(0);
    out_b_->set_level(0);
    out_w_->set_level(0);
    return;
  }

  float k = fade_level_;
  out_r_->set_level(r_ * k);
  out_g_->set_level(g_ * k);
  out_b_->set_level(b_ * k);
  out_w_->set_level(w_ * k);
}

}  // namespace super_rgbw
