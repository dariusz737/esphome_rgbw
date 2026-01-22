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

// ───── POWER ─────
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

// ───── RGBW ─────
void SuperRGBW::set_r(float v) {
  r_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (r_number_) r_number_->publish_state(r_);
  if (power_) render_();
}

void SuperRGBW::set_g(float v) {
  g_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (g_number_) g_number_->publish_state(g_);
  if (power_) render_();
}

void SuperRGBW::set_b(float v) {
  b_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (b_number_) b_number_->publish_state(b_);
  if (power_) render_();
}

void SuperRGBW::set_w(float v) {
  w_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (w_number_) w_number_->publish_state(w_);
  if (power_) render_();
}

// ───── DIM ─────
void SuperRGBW::set_dim(float v) {
  float target = clampf(v, DIM_FLOOR, 1.0f);
  apply_dim_(target);
  if (dim_number_) dim_number_->publish_state(dim_);
  if (power_) render_();
}

void SuperRGBW::loop() {

  // ───── FADE ─────
  if (fade_level_ != fade_target_) {
    uint32_t now = millis();
    float t = float(now - fade_start_ms_) / float(fade_time_ms_);

    if (t >= 1.0f) {
      fade_level_ = fade_target_;

      if (fading_off_) {
        power_ = false;
        fading_off_ = false;
      }
    } else {
      fade_level_ = fade_start_ + (fade_target_ - fade_start_) * t;
    }

    fade_level_ = clampf(fade_level_, 0.0f, 1.0f);
    render_();
  }

  // ───── DIM MANUAL (ZAWSZE SPRAWDZANY) ─────
  loop_dim_manual_();
}


void SuperRGBW::set_fade_time(uint32_t fade_ms) {
  fade_time_ms_ = std::max<uint32_t>(fade_ms, 1);
}

// ───── LOGIKA ─────
void SuperRGBW::update_dim_from_channels_() {
  if (dim_sync_lock_) return;

  dim_sync_lock_ = true;
  dim_ = std::max({r_, g_, b_, w_});
  if (dim_number_) dim_number_->publish_state(dim_);
  dim_sync_lock_ = false;
}

void SuperRGBW::apply_dim_(float target_dim) {
  if (dim_sync_lock_) return;
  dim_sync_lock_ = true;

  float max_v = std::max({r_, g_, b_, w_});
  if (max_v > 0.0f) {
    float scale = target_dim / max_v;
    if (r_ > 0.0f) r_ = clampf(r_ * scale, 0.0f, 1.0f);
    if (g_ > 0.0f) g_ = clampf(g_ * scale, 0.0f, 1.0f);
    if (b_ > 0.0f) b_ = clampf(b_ * scale, 0.0f, 1.0f);
    if (w_ > 0.0f) w_ = clampf(w_ * scale, 0.0f, 1.0f);
  }

  dim_ = clampf(target_dim, DIM_FLOOR, 1.0f);

  if (dim_number_) dim_number_->publish_state(dim_);

  if (r_number_) r_number_->publish_state(r_);
  if (g_number_) g_number_->publish_state(g_);
  if (b_number_) b_number_->publish_state(b_);
  if (w_number_) w_number_->publish_state(w_);

  dim_sync_lock_ = false;
}

void SuperRGBW::dim_manual_toggle() {
  if (!dim_manual_running_) {
    dim_manual_running_ = true;
    dim_manual_dir_up_ = !dim_manual_dir_up_;   // zmiana kierunku przy starcie
  } else {
    dim_manual_dir_up_ = !dim_manual_dir_up_;   // zmiana w trakcie
  }
}

void SuperRGBW::dim_manual_stop() {
  dim_manual_running_ = false;
  dim_cycle_finished_ = true;   // ⬅️ STOP kończy cykl
}


void SuperRGBW::loop_dim_manual_() {
  if (!dim_manual_running_) return;

  uint32_t now = millis();
  if (now - dim_manual_last_ms_ < 150) return;
  dim_manual_last_ms_ = now;

  const float STEP = 0.02f;
  const float DIM_FLOOR = 0.05f;

  float next = dim_ + (dim_manual_dir_up_ ? STEP : -STEP);

  if (next >= 1.0f) {
    next = 1.0f;
    dim_manual_running_ = false;
  }

  if (next <= DIM_FLOOR) {
    next = DIM_FLOOR;
    dim_manual_running_ = false;
  }

  apply_dim_(next);
  if (power_) render_();
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
