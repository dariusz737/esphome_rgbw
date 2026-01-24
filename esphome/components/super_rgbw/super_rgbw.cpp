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

  if (auto_ct_switch_) {
    auto_ct_switch_->add_on_state_callback(
      [this](bool state) {
        this->set_auto_ct_enabled(state);
      }
    );
  }

  if (r_number_) r_ = r_number_->state;
  if (g_number_) g_ = g_number_->state;
  if (b_number_) b_ = b_number_->state;
  if (w_number_) w_ = w_number_->state;
  if (dim_number_) dim_ = dim_number_->state;


  if (effect_fireplace_switch_) {
    effect_fireplace_switch_->add_on_state_callback(
      [this](bool state) {
        if (state) start_effect_fireplace();
        else stop_effect();
      }
    );
  }

  if (effect_alarm_switch_) {
    effect_alarm_switch_->add_on_state_callback(
      [this](bool state) {
        if (state) start_effect_alarm();
        else stop_effect();
      }
    );
  }

  render_();
}
                                                  // Loop

void SuperRGBW::loop() {

  // ──────────────── EFEKTY (NAJWYŻSZY PRIORYTET) ────────────────
  if (effect_running_) {
    if (effect_fireplace_switch_ && effect_fireplace_switch_->state) {
      loop_effect_fireplace_();
      return;
    }
    if (effect_alarm_switch_ && effect_alarm_switch_->state) {
      loop_effect_alarm_();
      return;
    }
  }

  // ──────────────── AUTO CT (tylko jeśli brak efektu) ────────────────
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

  // ──────────────── FADE ────────────────
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
  if (auto_ct_internal_change_) return;

  if (!auto_ct_running_) return;
  if (auto_ct_running_) {

    auto_ct_running_ = false;
    auto_ct_enabled_ = false;

    if (auto_ct_switch_) {
      auto_ct_switch_->publish_state(false);
    }
  }
}

void SuperRGBW::set_auto_ct_enabled(bool v) {
  if (!v) {
    maybe_cancel_auto_ct_();
    return;
  }
  auto_ct_enabled_ = true;
}

void SuperRGBW::handle_auto_ct_time_() {
  if (!auto_ct_enabled_) return;
  if (!time_) return;
  if (!auto_ct_start_min_) return;
  if (!auto_ct_duration_) return;

  auto now = time_->now();
  if (!now.is_valid()) return;

  int now_min = now.hour * 60 + now.minute;
  int start_min = int(auto_ct_start_min_->state);

  if (now_min != start_min) {
    last_auto_ct_min_ = -1;
    return;
  }

  if (last_auto_ct_min_ == now_min) return;
  last_auto_ct_min_ = now_min;

  uint32_t duration_ms =
      uint32_t(auto_ct_duration_->state) * 60000UL;

  auto_ct_start(duration_ms);
}

                                                  // Efekty

void SuperRGBW::start_effect_fireplace() {
  start_effect_common_(effect_fireplace_switch_);
}

void SuperRGBW::start_effect_alarm() {
  start_effect_common_(effect_alarm_switch_);
}


bool SuperRGBW::start_effect_common_(esphome::switch_::Switch *requesting_switch) {
  if (effect_running_) {
    if (requesting_switch)
      requesting_switch->publish_state(false);
    return false;
  }

  // przerwij Auto CT TYLKO jeśli AKTYWNIE BIEGNIE
  if (auto_ct_running_) {
    auto_ct_running_ = false;
    auto_ct_enabled_ = false;

    if (auto_ct_switch_) {
      auto_ct_switch_->publish_state(false);
    }
  }

  // zapamiętaj aktualny stan
  saved_r_ = r_;
  saved_g_ = g_;
  saved_b_ = b_;
  saved_w_ = w_;

  effect_running_ = true;
  return true;
}

void SuperRGBW::stop_effect() {
  stop_effect_common_();

  if (effect_fireplace_switch_)
    effect_fireplace_switch_->publish_state(false);
  if (effect_alarm_switch_)
    effect_alarm_switch_->publish_state(false);
}

void SuperRGBW::stop_effect_common_() {
  if (!effect_running_) return;

  effect_running_ = false;

  r_ = saved_r_;
  g_ = saved_g_;
  b_ = saved_b_;
  w_ = saved_w_;

  if (r_number_) r_number_->publish_state(r_);
  if (g_number_) g_number_->publish_state(g_);
  if (b_number_) b_number_->publish_state(b_);
  if (w_number_) w_number_->publish_state(w_);

  if (power_) render_();
}

void SuperRGBW::loop_effect_fireplace_() {
  uint32_t now = millis();
  if (now - effect_last_ms_ < 120) return;
  effect_last_ms_ = now;

  float d = dim_;
  if (d < 0.25f) d = 0.25f;

  float flicker = esphome::random_float();
  flicker = flicker * flicker;

  float w = d * (0.6f + 0.4f * flicker);
  float r = w * (0.35f + 0.15f * flicker);

  r_ = clampf(r, 0.0f, 1.0f);
  g_ = 0.0f;
  b_ = 0.0f;
  w_ = clampf(w, 0.0f, 1.0f);

  if (power_) render_();
}

void SuperRGBW::loop_effect_alarm_() {
  uint32_t now = millis();
  if (now - effect_last_ms_ < 120) return;
  effect_last_ms_ = now;

  static uint8_t step = 0;
  step = (step + 1) % 4;

  switch (step) {
    case 0: r_=1; g_=0; b_=0; w_=0; break;
    case 1: r_=1; g_=1; b_=1; w_=0; break;
    case 2: r_=0; g_=0; b_=1; w_=0; break;
    case 3: r_=1; g_=1; b_=1; w_=0; break;
  }

  if (power_) render_();
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
