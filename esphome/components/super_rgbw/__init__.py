                              # Imports for ESPHome code generation and validation

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output, number
from esphome.const import CONF_ID

                              # C++ namespace and class binding

super_rgbw_ns = cg.esphome_ns.namespace("super_rgbw")
SuperRGBW = super_rgbw_ns.class_("SuperRGBW", cg.Component)

                              # Output IDs for RGBW channels

CONF_OUT_R = "out_r"
CONF_OUT_G = "out_g"
CONF_OUT_B = "out_b"
CONF_OUT_W = "out_w"

                              # Number entities for channel state reporting

CONF_R_NUMBER = "r_number"
CONF_G_NUMBER = "g_number"
CONF_B_NUMBER = "b_number"
CONF_W_NUMBER = "w_number"
CONF_DIM_NUMBER = "dim_number"

                              # Optional fade configuration

CONF_FADE_TIME = "fade_time"

                              # Component configuration schema

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SuperRGBW),

        cv.Required(CONF_OUT_R): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_G): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_B): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_W): cv.use_id(output.FloatOutput),

        cv.Required(CONF_R_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_G_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_B_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_W_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_DIM_NUMBER): cv.use_id(number.Number),

        cv.Optional(CONF_FADE_TIME, default="1s"): cv.time_period,
    }
).extend(cv.COMPONENT_SCHEMA)


                              # Code generation: bind YAML config to C++ instance

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_out_r(await cg.get_variable(config[CONF_OUT_R])))
    cg.add(var.set_out_g(await cg.get_variable(config[CONF_OUT_G])))
    cg.add(var.set_out_b(await cg.get_variable(config[CONF_OUT_B])))
    cg.add(var.set_out_w(await cg.get_variable(config[CONF_OUT_W])))

    cg.add(var.set_r_number(await cg.get_variable(config[CONF_R_NUMBER])))
    cg.add(var.set_g_number(await cg.get_variable(config[CONF_G_NUMBER])))
    cg.add(var.set_b_number(await cg.get_variable(config[CONF_B_NUMBER])))
    cg.add(var.set_w_number(await cg.get_variable(config[CONF_W_NUMBER])))
    cg.add(var.set_dim_number(await cg.get_variable(config[CONF_DIM_NUMBER])))

    fade_ms = config[CONF_FADE_TIME].total_milliseconds
    cg.add(var.set_fade_time(fade_ms))
