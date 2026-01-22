#include "super_rgbw.h"
#include <algorithm>

                                                  // Pomocnicze
namespace super_rgbw {

static constexpr float DIM_FLOOR = 0.05f;

static inline float clampf(float v, float lo, float hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

                                                  // Setup
void SuperRGBW::setup() {
  if (r_number_) r_ = r_number_->state;
  if (g_number_) g_ = g_number_->state;
  if (b_number_) b_ = b_number_->state;
  if (w_number_) w_ = w_number_->state;
  if (dim_number_) dim_ = dim_number_->state;

  render_();
}

                                                  // Auto CT start
void SuperRGBW::auto_ct_start(uint32_t duration_ms) {
  if (!auto_ct_enabled_) return;

  auto_ct_running_ = true;
  auto_ct_step_ = 0;
  auto_ct_last_ms_ = millis();
  auto_ct_step_interval_ms_ = duration_ms / 30;

  auto_ct_r_start_ = r_;
  auto_ct_g_start_ = g_;
  auto_ct_b_start_ = b_;
  auto_ct_w_start_ = w_;
  auto_ct_dim_snapshot_ = dim_;
}

                                                  // Loop
void SuperRGBW::loop() {

  if (auto_ct_running_) {
    uint32_t now = millis();
    if (now - auto_ct_last_ms_ >= auto_ct_step_interval_ms_) {
      auto_ct_last_ms_ = now;
      auto_ct_step_++;

      float k = float(auto_ct_step_) / 30.0f;

      auto_ct_internal_change_ = true;

      r_ = auto_ct_r_start_ * (1.0f - k);
      g_ = auto_ct_g_start_ * (1.0f - k);
      b_ = auto_ct_b_start_ * (1.0f - k);
      w_ = auto_ct_w_start_ + (auto_ct_dim_snapshot_ - auto_ct_w_start_) * k;

      if (r_number_) r_number_->publish_state(r_);
      if (g_number_) g_number_->publish_state(g_);
      if (b_number_) b_number_->publish_state(b_);
      if (w_number_) w_number_->publish_state(w_);

      auto_ct_internal_change_ = false;

      if (power_) render_();

      if (auto_ct_step_ >= 30) {
        auto_ct_running_ = false;
      }
    }
  }

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

  loop_dim_manual_();
  handle_auto_ct_time_();
}


                                                  // Power
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

                                                  // RGBW
void SuperRGBW::set_r(float v) {

  maybe_cancel_auto_ct_();

  r_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (r_number_) r_number_->publish_state(r_);
  if (power_) render_();
}



void SuperRGBW::set_g(float v) {

  maybe_cancel_auto_ct_();

  g_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (g_number_) g_number_->publish_state(g_);
  if (power_) render_();
}

void SuperRGBW::set_b(float v) {

  maybe_cancel_auto_ct_();

  b_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (b_number_) b_number_->publish_state(b_);
  if (power_) render_();
}

void SuperRGBW::set_w(float v) {

  maybe_cancel_auto_ct_();

  w_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  if (w_number_) w_number_->publish_state(w_);
  if (power_) render_();
}

                                                  // DIM
void SuperRGBW::set_dim(float v) {

  maybe_cancel_auto_ct_();

  apply_dim_(clampf(v, DIM_FLOOR, 1.0f));
  if (dim_number_) dim_number_->publish_state(dim_);
  if (power_) render_();
}

                                                  // DIM z kanalow
void SuperRGBW::update_dim_from_channels_() {
  if (dim_sync_lock_) return;
  dim_sync_lock_ = true;

  dim_ = std::max({r_, g_, b_, w_});
  if (dim_number_) dim_number_->publish_state(dim_);

  dim_sync_lock_ = false;
}

                                                  // Skalowanie DIM
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

  dim_ = target_dim;

  if (r_number_) r_number_->publish_state(r_);
  if (g_number_) g_number_->publish_state(g_);
  if (b_number_) b_number_->publish_state(b_);
  if (w_number_) w_number_->publish_state(w_);
  if (dim_number_) dim_number_->publish_state(dim_);

  dim_sync_lock_ = false;
}

                                                  // Reczne dimowanie
void SuperRGBW::dim_manual_toggle() {
  if (!dim_manual_running_) {
    dim_manual_running_ = true;
    dim_manual_dir_up_ = (dim_ < 0.5f);
  } else {
    dim_manual_dir_up_ = !dim_manual_dir_up_;
  }
}

void SuperRGBW::dim_manual_stop() {
  dim_manual_running_ = false;
}

void SuperRGBW::loop_dim_manual_() {
  if (!dim_manual_running_) return;

  uint32_t now = millis();
  if (now - dim_manual_last_ms_ < 150) return;
  dim_manual_last_ms_ = now;

  float next = dim_ + (dim_manual_dir_up_ ? 0.02f : -0.02f);

  if (next >= 1.0f || next <= DIM_FLOOR) {
    next = clampf(next, DIM_FLOOR, 1.0f);
    dim_manual_running_ = false;
  }

  apply_dim_(next);
  if (power_) render_();
}

                                                  // Sceny
void SuperRGBW::set_scene(Scene scene) {

  maybe_cancel_auto_ct_();

  current_scene_ = scene;
  float d = dim_;

  switch (scene) {
    case SCENE_COLD:    r_ = d; g_ = d; b_ = d; w_ = 0; break;
    case SCENE_NEUTRAL: r_ = d; g_ = d; b_ = d; w_ = d; break;
    case SCENE_WARM:    r_ = 0; g_ = 0; b_ = 0; w_ = d; break;
  }

  if (r_number_) r_number_->publish_state(r_);
  if (g_number_) g_number_->publish_state(g_);
  if (b_number_) b_number_->publish_state(b_);
  if (w_number_) w_number_->publish_state(w_);
  if (dim_number_) dim_number_->publish_state(dim_);

  if (power_) render_();
}

void SuperRGBW::next_scene() {
  set_scene(current_scene_ == SCENE_WARM ? SCENE_COLD : Scene(current_scene_ + 1));
}

void SuperRGBW::scene_cold()    { set_scene(SCENE_COLD); }
void SuperRGBW::scene_neutral() { set_scene(SCENE_NEUTRAL); }
void SuperRGBW::scene_warm()    { set_scene(SCENE_WARM); }


                                                  // Przerwanie Auto CT przez uzytkownika
void SuperRGBW::maybe_cancel_auto_ct_() {
  if (auto_ct_running_ && !auto_ct_internal_change_) {
    auto_ct_running_ = false;
    auto_ct_enabled_ = false;

    if (auto_ct_switch_) {
      auto_ct_switch_->publish_state(false);
    }
  }
}

void SuperRGBW::handle_auto_ct_time_() {
  if (!auto_ct_enabled_) {
    ESP_LOGD("super_rgbw", "Auto CT disabled");
    return;
  }
  if (!time_) {
    ESP_LOGD("super_rgbw", "No time component");
    return;
  }
  if (!auto_ct_start_min_) {
    ESP_LOGD("super_rgbw", "No start_min number");
    return;
  }
  if (!auto_ct_duration_) {
    ESP_LOGD("super_rgbw", "No duration number");
    return;
  }

  auto now = time_->now();
  if (!now.is_valid()) {
    ESP_LOGD("super_rgbw", "Time not valid yet");
    return;
  }

  int now_min = now.hour * 60 + now.minute;
  int start_min = int(auto_ct_start_min_->state);

  ESP_LOGD(
    "super_rgbw",
    "Time check: now=%02d:%02d (%d) start_min=%d last_day=%d today=%d",
    now.hour,
    now.minute,
    now_min,
    start_min,
    last_auto_ct_day_,
    now.day_of_year
  );

  if (now_min < start_min) return;
  if (now_min > start_min + 1) return;

  if (now.day_of_year == last_auto_ct_day_) {
    ESP_LOGD("super_rgbw", "Already started today");
    return;
  }

  last_auto_ct_day_ = now.day_of_year;

  uint32_t duration_ms =
      uint32_t(auto_ct_duration_->state) * 60000UL;

  ESP_LOGI(
    "super_rgbw",
    "AUTO CT START: duration=%u ms",
    duration_ms
  );

  auto_ct_start(duration_ms);
}
                                                  // Render
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

                                                  // Fade time
void SuperRGBW::set_fade_time(uint32_t fade_ms) {
  fade_time_ms_ = std::max<uint32_t>(fade_ms, 1);
}

}  // namespace super_rgbw
