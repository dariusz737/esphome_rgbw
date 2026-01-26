#error "SUPER_RGBW HEADER USED"

#pragma once

#include "esphome.h"
#include "esphome/components/time/real_time_clock.h"

namespace super_rgbw {

//
// ============================================================
//  KTO MA KONTROLĘ NAD ŚWIATŁEM (arbiter)
// ============================================================
//
enum ScriptType {
  SCRIPT_NONE,
  SCRIPT_DIM_MANUAL,   // ręczne dimowanie (przycisk)
  SCRIPT_AUTO_CT,      // automatyczna zmiana CT (czas)
  SCRIPT_VISUAL        // efekt wizualny (blokujący)
};

//
// ============================================================
//  EFEKTY WIZUALNE (tylko gdy SCRIPT_VISUAL)
// ============================================================
//
enum VisualEffect {
  VISUAL_NONE,
  VISUAL_FIREPLACE,
  VISUAL_ALARM
};

//
// ============================================================
//  GŁÓWNY KOMPONENT
// ============================================================
//
class SuperRGBW : public esphome::Component {
 public:

   void __THIS_METHOD_MUST_EXIST__();
  // ------------------------------------------------------------
  //  PUBLICZNE API (używane w YAML / lambdach)
  // ------------------------------------------------------------

  // Efekty wizualne (bez enumów w YAML)
// Efekty wizualne – API
  void start_fireplace();
  void start_alarm();
  void stop_visual();§

  // Ręczne sterowanie jasnością
  void dim_manual_toggle();
  void dim_manual_stop();

  // Auto CT
  void set_auto_ct_enabled(bool v);

  // Zasilanie (fade in/out)
  void set_power(bool on);

  // Kanały RGBW + DIM
  void set_r(float v);
  void set_g(float v);
  void set_b(float v);
  void set_w(float v);
  void set_dim(float v);

  // Sceny kolorystyczne
  void set_scene(Scene scene);
  void next_scene();
  void scene_cold();
  void scene_neutral();
  void scene_warm();

  // ------------------------------------------------------------
  //  ŻYCIE KOMPONENTU ESPHOME
  // ------------------------------------------------------------
  void setup() override;
  void loop() override;

  // ------------------------------------------------------------
  //  WSTRZYKIWANIE Z CODEGEN (PY)
  // ------------------------------------------------------------
  void set_out_r(esphome::output::FloatOutput *out) { out_r_ = out; }
  void set_out_g(esphome::output::FloatOutput *out) { out_g_ = out; }
  void set_out_b(esphome::output::FloatOutput *out) { out_b_ = out; }
  void set_out_w(esphome::output::FloatOutput *out) { out_w_ = out; }

  void set_r_number(esphome::number::Number *n) { r_number_ = n; }
  void set_g_number(esphome::number::Number *n) { g_number_ = n; }
  void set_b_number(esphome::number::Number *n) { b_number_ = n; }
  void set_w_number(esphome::number::Number *n) { w_number_ = n; }
  void set_dim_number(esphome::number::Number *n) { dim_number_ = n; }

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

  void set_fade_time(uint32_t fade_ms);

 protected:
  // ------------------------------------------------------------
  //  ARBITER STEROWANIA (JEDNO MIEJSCE DECYZJI)
  // ------------------------------------------------------------
  bool start_script_(ScriptType s);
  void stop_script_();

  // ------------------------------------------------------------
  //  EFEKTY WIZUALNE (wewnętrzne)
  // ------------------------------------------------------------
  void start_visual_(VisualEffect v);

  // ------------------------------------------------------------
  //  RENDER I FADE
  // ------------------------------------------------------------
  void render_();

  // ------------------------------------------------------------
  //  DIM (logika wewnętrzna)
  // ------------------------------------------------------------
  void update_dim_from_channels_();
  void apply_dim_(float target_dim);
  void loop_dim_manual_();

  // ------------------------------------------------------------
  //  AUTO CT (logika)
  // ------------------------------------------------------------
  void auto_ct_start(uint32_t duration_ms);
  void loop_effect_auto_ct_();
  void handle_auto_ct_time_();

  // ------------------------------------------------------------
  //  EFEKTY WIZUALNE – LOOP
  // ------------------------------------------------------------
  void loop_effect_fireplace_();
  void loop_effect_alarm_();

  // ------------------------------------------------------------
  //  STAN OGÓLNY
  // ------------------------------------------------------------
  ScriptType   active_script_{SCRIPT_NONE};
  VisualEffect active_visual_{VISUAL_NONE};

  bool power_{false};
  bool dim_sync_lock_{false};

  // RGBW + DIM
  float r_{0.0f};
  float g_{0.0f};
  float b_{0.0f};
  float w_{0.0f};
  float dim_{0.0f};

  // ------------------------------------------------------------
  //  FADE
  // ------------------------------------------------------------
  float fade_start_{0.0f};
  float fade_target_{0.0f};
  float fade_level_{0.0f};
  uint32_t fade_start_ms_{0};
  uint32_t fade_time_ms_{1000};
  bool fading_off_{false};

  // ------------------------------------------------------------
  //  AUTO CT – PARAMETRY I STAN
  // ------------------------------------------------------------
  bool auto_ct_enabled_{false};

  uint8_t auto_ct_step_{0};
  uint32_t auto_ct_last_ms_{0};
  uint32_t auto_ct_step_interval_ms_{0};

  float auto_ct_r_start_{0};
  float auto_ct_g_start_{0};
  float auto_ct_b_start_{0};
  float auto_ct_w_start_{0};
  float auto_ct_dim_snapshot_{0};

  int last_auto_ct_min_{-1};

  // ------------------------------------------------------------
  //  EFEKTY – ZAPAMIĘTANY STAN
  // ------------------------------------------------------------
  float saved_r_{0};
  float saved_g_{0};
  float saved_b_{0};
  float saved_w_{0};

  bool effect_forced_power_{false};

  // ------------------------------------------------------------
  //  ZALEŻNOŚCI (wstrzykiwane z ESPHome)
  // ------------------------------------------------------------
  esphome::output::FloatOutput *out_r_{nullptr};
  esphome::output::FloatOutput *out_g_{nullptr};
  esphome::output::FloatOutput *out_b_{nullptr};
  esphome::output::FloatOutput *out_w_{nullptr};

  esphome::number::Number *r_number_{nullptr};
  esphome::number::Number *g_number_{nullptr};
  esphome::number::Number *b_number_{nullptr};
  esphome::number::Number *w_number_{nullptr};
  esphome::number::Number *dim_number_{nullptr};

  esphome::switch_::Switch *auto_ct_switch_{nullptr};
  esphome::time::RealTimeClock *time_{nullptr};
  esphome::number::Number *auto_ct_start_min_{nullptr};
  esphome::number::Number *auto_ct_duration_{nullptr};

  // ------------------------------------------------------------
  //  TIMERY EFEKTÓW
  // ------------------------------------------------------------
  uint32_t effect_last_ms_{0};
  uint8_t effect_step_{0};
};

}  // namespace super_rgbw
