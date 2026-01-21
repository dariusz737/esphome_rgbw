import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID

DEPENDENCIES = ["output"]
AUTO_LOAD = ["output"]

super_rgbw_ns = cg.esphome_ns.namespace("super_rgbw")
SuperRGBW = super_rgbw_ns.class_("SuperRGBW", cg.Component)

CONF_OUT_R = "out_r"
CONF_OUT_G = "out_g"
CONF_OUT_B = "out_b"
CONF_OUT_W = "out_w"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SuperRGBW),

        cv.Required(CONF_OUT_R): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_G): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_B): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_W): cv.use_id(output.FloatOutput),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_out_r(await cg.get_variable(config[CONF_OUT_R])))
    cg.add(var.set_out_g(await cg.get_variable(config[CONF_OUT_G])))
    cg.add(var.set_out_b(await cg.get_variable(config[CONF_OUT_B])))
    cg.add(var.set_out_w(await cg.get_variable(config[CONF_OUT_W])))
