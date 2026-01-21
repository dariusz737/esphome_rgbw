import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

# --- WYMUSZENIE ZAŁADOWANIA ZALEŻNOŚCI ---
DEPENDENCIES = ["output"]
AUTO_LOAD = ["output"]

# --- NAMESPACE C++ ---
super_rgbw_ns = cg.esphome_ns.namespace("super_rgbw")
SuperRGBW = super_rgbw_ns.class_("SuperRGBW", cg.Component)

# --- KLUCZE YAML ---
CONF_OUT_R = "out_r"
CONF_OUT_G = "out_g"
CONF_OUT_B = "out_b"
CONF_OUT_W = "out_w"

# --- SCHEMA YAML ---
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SuperRGBW),

        cv.Required(CONF_OUT_R): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_G): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_B): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_W): cv.use_id(output.FloatOutput),
    }
).extend(cv.COMPONENT_SCHEMA)

# --- GENERACJA KODU C++ ---
async def to_code(config):
    # 🔴 KLUCZOWE: pełna definicja klasy widoczna dla lambd
    cg.add_global(cg.RawExpression('#include "super_rgbw.h"'))

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    out_r = await cg.get_variable(config[CONF_OUT_R])
    out_g = await cg.get_variable(config[CONF_OUT_G])
    out_b = await cg.get_variable(config[CONF_OUT_B])
    out_w = await cg.get_variable(config[CONF_OUT_W])

    cg.add(var.set_out_r(out_r))
    cg.add(var.set_out_g(out_g))
    cg.add(var.set_out_b(out_b))
    cg.add(var.set_out_w(out_w))
