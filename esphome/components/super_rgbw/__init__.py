import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import number, output, switch, time
from esphome.const import CONF_ID

                                                  # Namespace i klasa C++ komponentu
super_rgbw_ns = cg.esphome_ns.namespace("super_rgbw")
SuperRGBW = super_rgbw_ns.class_("SuperRGBW", cg.Component)

                                                  # Klucze konfiguracyjne
CONF_OUT_R = "out_r"
CONF_OUT_G = "out_g"
CONF_OUT_B = "out_b"
CONF_OUT_W = "out_w"

CONF_R_NUMBER = "r_number"
CONF_G_NUMBER = "g_number"
CONF_B_NUMBER = "b_number"
CONF_W_NUMBER = "w_number"
CONF_DIM_NUMBER = "dim_number"

CONF_FADE_TIME = "fade_time"
CONF_AUTO_CT_SWITCH = "auto_ct_switch"

CONF_TIME_ID = "time_id"

CONF_AUTO_CT_START_MIN = "auto_ct_start_min"
CONF_AUTO_CT_DURATION = "auto_ct_duration"

                                                  # Schemat konfiguracji komponentu
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
        cv.Optional(CONF_AUTO_CT_SWITCH): cv.use_id(switch.Switch),

        cv.Required(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Required(CONF_AUTO_CT_START_MIN): cv.use_id(number.Number),
        cv.Required(CONF_AUTO_CT_DURATION): cv.use_id(number.Number),

    }
).extend(cv.COMPONENT_SCHEMA)

                                                  # Generowanie kodu C++
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

                                                  # Wyjscia PWM
    cg.add(var.set_out_r(await cg.get_variable(config[CONF_OUT_R])))
    cg.add(var.set_out_g(await cg.get_variable(config[CONF_OUT_G])))
    cg.add(var.set_out_b(await cg.get_variable(config[CONF_OUT_B])))
    cg.add(var.set_out_w(await cg.get_variable(config[CONF_OUT_W])))

                                                  # Encje Number
    cg.add(var.set_r_number(await cg.get_variable(config[CONF_R_NUMBER])))
    cg.add(var.set_g_number(await cg.get_variable(config[CONF_G_NUMBER])))
    cg.add(var.set_b_number(await cg.get_variable(config[CONF_B_NUMBER])))
    cg.add(var.set_w_number(await cg.get_variable(config[CONF_W_NUMBER])))
    cg.add(var.set_dim_number(await cg.get_variable(config[CONF_DIM_NUMBER])))

                                                  # Konfiguracja fade
    cg.add(var.set_fade_time(config[CONF_FADE_TIME].total_milliseconds))

                                                  # Opcjonalny switch Auto CT
    if CONF_AUTO_CT_SWITCH in config:
        cg.add(
            var.set_auto_ct_switch(
                await cg.get_variable(config[CONF_AUTO_CT_SWITCH])
            )
        )
                                                  # Auto CT (czas i parametry)
    cg.add(var.set_time(
        await cg.get_variable(config[CONF_TIME_ID])
    ))
    cg.add(
        var.set_auto_ct_start_min_number(
            await cg.get_variable(config[CONF_AUTO_CT_START_MIN])
        )
    )

    cg.add(
        var.set_auto_ct_duration_number(
            await cg.get_variable(config[CONF_AUTO_CT_DURATION])
        )
    )