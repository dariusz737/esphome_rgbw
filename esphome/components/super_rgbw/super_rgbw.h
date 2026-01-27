#pragma once
#include "esphome.h"
#include "esphome/components/time/real_time_clock.h"

namespace super_rgbw {

  enum Scene {
    SCENE_COLD,
    SCENE_NEUTRAL,
    SCENE_WARM
  };

  enum EffectType {
    EFFECT_NONE,
    EFFECT_FIREPLACE,
    EFFECT_ALARM
  };

class SuperRGBW : public esphome::Component {
 public:

  // Cykl zycia ESPHome
  void setup() override;
  void loop() override;

  // Zasilanie (fade)
  void set_power(bool on);

  // Kanaly RGBW i DIM
  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);
  void set_dim(float v);

  // Sceny
  void set_scene(Scene scene);
  void next_scene();
  void scene_cold();
  void scene_neutral();
  void scene_warm();

  // Wyjscia PWM
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  // Encje Number
  void set_r_number(esphome::number::Number *n) { r_number_ = n; }
  void set_g_number(esphome::number::Number *n) { g_number_ = n; }
  void set_b_number(esphome::number::Number *n) { b_number_ = n; }
  void set_w_number(esphome::number::Number *n) { w_number_ = n; }
  void set_dim_number(esphome::number::Number *n) { dim_number_ = n; }

  // Parametry
  void set_fade_time(uint32_t fade_ms);

  // Reczne dimowanie
  void dim_manual_toggle();
  void dim_manual_stop();

  // Auto CT
  void set_auto_ct_enabled(bool v);
  void auto_ct_start(uint32_t duration_ms);

  void set_auto_ct_switch(esphome::switch_::Switch *s) {
    auto_ct_switch_ = s;
  }

  void set_time(esphome::time::RealTimeClock *t) { time_ = t; }

  void set_auto_ct_start_min_number(esphome::number::Number *n) {
    auto_ct_start_min_ = n;
  }

  void set_auto_ct_duration_number(esphome::number::Number *n) {
    auto_ct_duration_ = n;
  }

  // Power / DIM inputy
  void set_power_switch(esphome::switch_::Switch *s) { power_switch_ = s; }
  void set_dim_start(esphome::binary_sensor::BinarySensor *b) { dim_start_ = b; }
  void set_dim_stop(esphome::binary_sensor::BinarySensor *b) { dim_stop_ = b; }

  // Buttony scen
  void set_scene_cold_button(esphome::button::Button *b) { scene_cold_button_ = b; }
  void set_scene_neutral_button(esphome::button::Button *b) { scene_neutral_button_ = b; }
  void set_scene_warm_button(esphome::button::Button *b) { scene_warm_button_ = b; }
  void set_scene_next_button(esphome::button::Button *b) { scene_next_button_ = b; }

  // Buttony efektow
  void set_effect_fireplace_button(esphome::button::Button *b) { effect_fireplace_button_ = b; }
  void set_effect_alarm_button(esphome::button::Button *b) { effect_alarm_button_ = b; }
  void set_effect_stop_button(esphome::button::Button *b) { effect_stop_button_ = b; }

  // Efekty
  void start_effect(EffectType requested);
  void stop_effect(EffectType requested);

 protected:

  // Render PWM
  void render_();

  // Logika DIM
  void update_dim_from_channels_();
  void apply_dim_(float target_dim);
  void loop_dim_manual_();
  void dim_stop_forced_();

  // Auto CT
  void maybe_cancel_auto_ct_();
  void handle_auto_ct_time_();

  // Aktualna scena
  Scene current_scene_{SCENE_NEUTRAL};

  // Wyjscia
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  // Number
  esphome::number::Number *r_number_{nullptr};
  esphome::number::Number *g_number_{nullptr};
  esphome::number::Number *b_number_{nullptr};
  esphome::number::Number *w_number_{nullptr};
  esphome::number::Number *dim_number_{nullptr};

  // Switche / sensory
  esphome::switch_::Switch *power_switch_{nullptr};
  esphome::switch_::Switch *auto_ct_switch_{nullptr};

  esphome::binary_sensor::BinarySensor *dim_start_{nullptr};
  esphome::binary_sensor::BinarySensor *dim_stop_{nullptr};

  // Buttony scen
  esphome::button::Button *scene_cold_button_{nullptr};
  esphome::button::Button *scene_neutral_button_{nullptr};
  esphome::button::Button *scene_warm_button_{nullptr};
  esphome::button::Button *scene_next_button_{nullptr};

  // Buttony efektow
  esphome::button::Button *effect_fireplace_button_{nullptr};
  esphome::button::Button *effect_alarm_button_{nullptr};
  esphome::button::Button *effect_stop_button_{nullptr};

  // Czas / Auto CT
  esphome::time::RealTimeClock *time_{nullptr};
  esphome::number::Number *auto_ct_start_min_{nullptr};
  esphome::number::Number *auto_ct_duration_{nullptr};

  // Stan
  bool power_{false};
  bool dim_sync_lock_{false};

  // RGBW + DIM
  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float w_{0.0f};
  float dim_{0.0f};

  // Fade
  float fade_start_{0.0f};
  float fade_target_{0.0f};
  float fade_level_{0.0f};
  uint32_t fade_start_ms_{0};
  uint32_t fade_time_ms_{1000};
  bool fading_off_{false};

  // Reczne dimowanie
  bool dim_manual_running_{false};
  bool dim_manual_dir_up_{true};
  uint32_t dim_manual_last_ms_{0};

  // Auto CT runtime
  bool auto_ct_enabled_{false};
  bool auto_ct_running_{false};
  bool auto_ct_internal_change_{false};

  uint8_t auto_ct_step_{0};
  uint32_t auto_ct_last_ms_{0};
  uint32_t auto_ct_step_interval_ms_{0};

  float auto_ct_r_start_{0};
  float auto_ct_g_start_{0};
  float auto_ct_b_start_{0};
  float auto_ct_w_start_{0};
  float auto_ct_dim_snapshot_{0};

  int last_auto_ct_min_{-1};

  // Efekty
  void start_effect_common_(EffectType requested);

  float saved_r_{0};
  float saved_g_{0};
  float saved_b_{0};
  float saved_w_{0};

  void set_power_immediate_(bool on);
  bool effect_forced_power_{false};

  void loop_effect_fireplace_();
  void loop_effect_alarm_();

  uint32_t effect_last_ms_{0};
  uint8_t effect_step_{0};

  EffectType current_effect_{EFFECT_NONE};
};

}  // namespace super_rgbw
