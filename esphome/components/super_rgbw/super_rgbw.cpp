#include "super_rgbw.h"
#include <algorithm>

namespace super_rgbw {

                                                  // Pomocnicze
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

  render_();
}
                                                  // Loop
void SuperRGBW::loop() {

  // --- arbiter sterowania ---
  switch (active_script_) {

    case SCRIPT_DIM_MANUAL:
      loop_dim_manual_();
      break;

    case SCRIPT_AUTO_CT:
      loop_effect_auto_ct_();
      break;

    case SCRIPT_VISUAL:
      switch (active_visual_) {
        case VISUAL_FIREPLACE:
          loop_effect_fireplace_();
          break;

        case VISUAL_ALARM:
          loop_effect_alarm_();
          break;

        default:
          break;
      }
      break;

    case SCRIPT_NONE:
    default:
      break;
  }

  // --- fade (ZAWSZE) ---
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

  // --- tylko TRIGGER auto CT ---
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
                                                    // Power natychmiast bez fade
void SuperRGBW::set_power_immediate_(bool on) {
  power_ = on;
  fade_level_ = on ? 1.0f : 0.0f;
  fade_target_ = fade_level_;
  fading_off_ = false;
  render_();
}
                                                  // RGBW
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

void SuperRGBW::set_dim(float v) {

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
  if (active_script_ != SCRIPT_DIM_MANUAL) {
    start_script_(SCRIPT_DIM_MANUAL);
    dim_manual_dir_up_ = (dim_ < 0.5f);
  } else {
    dim_manual_dir_up_ = !dim_manual_dir_up_;
  }
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

void SuperRGBW::dim_manual_stop() {
  if (active_script_ == SCRIPT_DIM_MANUAL)
    stop_script_();
}


                                                  // Sceny
void SuperRGBW::set_scene(Scene scene) {

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


                                                  // Auto CT wlacz/wy
void SuperRGBW::set_auto_ct_enabled(bool v) {
  if (!v) {
    return;
  }
  auto_ct_enabled_ = true;
}

                                                  // Logika Auto CT (timer)
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

  if (start_script_(SCRIPT_AUTO_CT)) {
    auto_ct_start(duration_ms);
  }

}

bool SuperRGBW::start_script_(ScriptType s) {
  if (active_script_ == s) return true;

  // wizualny blokuje wszystko
  if (active_script_ == SCRIPT_VISUAL)
    return false;

  stop_script_();

  if (s == SCRIPT_VISUAL) {
    save_state_();
    if (!power_) {
      effect_forced_power_ = true;
      set_power_immediate_(true);
    }
  }

  active_script_ = s;
  return true;
}

void SuperRGBW::stop_script_() {
  if (active_script_ == SCRIPT_VISUAL) {
    restore_state_();
    if (effect_forced_power_) {
      set_power_immediate_(false);
      effect_forced_power_ = false;
    }
  }

  active_script_ = SCRIPT_NONE;
  active_visual_ = VISUAL_NONE;
}


                                                  // Efekty

void SuperRGBW::start_visual_(VisualEffect v) {
  if (!start_script_(SCRIPT_VISUAL)) return;
  active_visual_ = v;
}


void SuperRGBW::start_effect_common_(EffectType requested) {
  if (current_effect_ != EFFECT_NONE) {
    return; // inny efekt już działa
  }

  if (auto_ct_running_) {
    auto_ct_running_ = false;
    auto_ct_enabled_ = false;
    if (auto_ct_switch_)
      auto_ct_switch_->publish_state(false);
  }

  // zapamiętaj stan
  saved_r_ = r_;
  saved_g_ = g_;
  saved_b_ = b_;
  saved_w_ = w_;

  // jeśli światło było zgaszone → efekt wymusza power
  if (!power_) {
    effect_forced_power_ = true;
    set_power_immediate_(true);
  }

current_effect_ = requested;

}


void SuperRGBW::stop_effect(EffectType requested) {
  if (current_effect_ == EFFECT_NONE) return;

  current_effect_ = EFFECT_NONE;

  r_ = saved_r_;
  g_ = saved_g_;
  b_ = saved_b_;
  w_ = saved_w_;

  // przywróć stan RGBW
  r_ = saved_r_;
  g_ = saved_g_;
  b_ = saved_b_;
  w_ = saved_w_;

  if (r_number_) r_number_->publish_state(r_);
  if (g_number_) g_number_->publish_state(g_);
  if (b_number_) b_number_->publish_state(b_);
  if (w_number_) w_number_->publish_state(w_);

  // jeśli efekt włączył światło → zgaś je
  if (effect_forced_power_) {
    set_power_immediate_(false);
    effect_forced_power_ = false;
  } else if (power_) {
    render_();
  }
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

}
