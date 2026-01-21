#include "super_rgbw.h"
#include <algorithm>

namespace super_rgbw {

// ───── helper ─────
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
  power_ = on;
  render_();
}

// ───── KANAŁY ─────

void SuperRGBW::set_r(float v) {
  r_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

void SuperRGBW::set_g(float v) {
  g_ = clampf(v, 0.0f, 1.0f);
  update_dim_from_channels_();
  render_();
}

void SuperRGBW::set_b(float v) {
  b_ = clampf(v, 0._
