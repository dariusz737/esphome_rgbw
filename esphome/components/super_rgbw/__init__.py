print("SUPER_RGBW LOADED – NEW SCHEMA")
import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import (
    number,
    output,
    switch,
    time,
    button,
    binary_sensor,
)

from esphome.const import CONF_ID

                                                  # Namespace i klasa C++ komponentu
super_rgbw_ns = cg.esphome_ns.namespace("super_rgbw")
SuperRGBW = super_rgbw_ns.class_("SuperRGBW", cg.Component)

                                                  # Klucze konfiguracyjne
CONF_TIME_ID = "time_id"
CONF_FADE_TIME = "fade_time"

CONF_POWER_SWITCH = "power_switch"

CONF_OUT_R = "out_r"
CONF_OUT_G = "out_g"
CONF_OUT_B = "out_b"
CONF_OUT_W = "out_w"

CONF_R_NUMBER = "r_number"
CONF_G_NUMBER = "g_number"
CONF_B_NUMBER = "b_number"
CONF_W_NUMBER = "w_number"
CONF_DIM_NUMBER = "dim_number"

CONF_DIM_START = "dim_start"
CONF_DIM_STOP = "dim_stop"

CONF_AUTO_CT_START_MIN = "auto_ct_start_min"
CONF_AUTO_CT_DURATION = "auto_ct_duration"
CONF_AUTO_CT_SWITCH = "auto_ct_switch"

CONF_SCENE_COLD_BUTTON = "scene_cold_button"
CONF_SCENE_NEUTRAL_BUTTON = "scene_neutral_button"
CONF_SCENE_WARM_BUTTON = "scene_warm_button"
CONF_SCENE_NEXT_BUTTON = "scene_next_button"

CONF_EFFECT_FIREPLACE_BUTTON = "effect_fireplace_button"
CONF_EFFECT_ALARM_BUTTON = "effect_alarm_button"
CONF_EFFECT_STOP_BUTTON = "effect_stop_button"

                                                  # Schemat konfiguracji komponentu
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SuperRGBW),

        cv.Required(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
        cv.Optional(CONF_FADE_TIME, default="1s"): cv.time_period,

        cv.Optional(CONF_POWER_SWITCH): cv.use_id(switch.Switch),

        cv.Required(CONF_OUT_R): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_G): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_B): cv.use_id(output.FloatOutput),
        cv.Required(CONF_OUT_W): cv.use_id(output.FloatOutput),

        cv.Required(CONF_R_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_G_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_B_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_W_NUMBER): cv.use_id(number.Number),
        cv.Required(CONF_DIM_NUMBER): cv.use_id(number.Number),

        cv.Optional(CONF_DIM_START): cv.use_id(binary_sensor.BinarySensor),
        cv.Optional(CONF_DIM_STOP): cv.use_id(binary_sensor.BinarySensor),

        cv.Required(CONF_AUTO_CT_START_MIN): cv.use_id(number.Number),
        cv.Required(CONF_AUTO_CT_DURATION): cv.use_id(number.Number),
        cv.Optional(CONF_AUTO_CT_SWITCH): cv.use_id(switch.Switch),

        cv.Optional(CONF_SCENE_COLD_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_SCENE_NEUTRAL_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_SCENE_WARM_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_SCENE_NEXT_BUTTON): cv.use_id(button.Button),

        cv.Optional(CONF_EFFECT_FIREPLACE_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_EFFECT_ALARM_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_EFFECT_STOP_BUTTON): cv.use_id(button.Button),
    }
).extend(cv.COMPONENT_SCHEMA)

                                                  # Generowanie kodu C++
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # time
    cg.add(
        var.set_time(
            await cg.get_variable(config[CONF_TIME_ID])
        )
    )

    # fade
    cg.add(
        var.set_fade_time(
            config[CONF_FADE_TIME].total_milliseconds
        )
    )

    # power switch
    if CONF_POWER_SWITCH in config:
        cg.add(
            var.set_power_switch(
                await cg.get_variable(config[CONF_POWER_SWITCH])
            )
        )

    # PWM outputs
    cg.add(var.set_out_r(await cg.get_variable(config[CONF_OUT_R])))
    cg.add(var.set_out_g(await cg.get_variable(config[CONF_OUT_G])))
    cg.add(var.set_out_b(await cg.get_variable(config[CONF_OUT_B])))
    cg.add(var.set_out_w(await cg.get_variable(config[CONF_OUT_W])))

    # number entities
    cg.add(var.set_r_number(await cg.get_variable(config[CONF_R_NUMBER])))
    cg.add(var.set_g_number(await cg.get_variable(config[CONF_G_NUMBER])))
    cg.add(var.set_b_number(await cg.get_variable(config[CONF_B_NUMBER])))
    cg.add(var.set_w_number(await cg.get_variable(config[CONF_W_NUMBER])))
    cg.add(var.set_dim_number(await cg.get_variable(config[CONF_DIM_NUMBER])))

    # dim buttons
    if CONF_DIM_START in config:
        cg.add(
            var.set_dim_start(
                await cg.get_variable(config[CONF_DIM_START])
            )
        )

    if CONF_DIM_STOP in config:
        cg.add(
            var.set_dim_stop(
                await cg.get_variable(config[CONF_DIM_STOP])
            )
        )

    # auto CT params
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

    if CONF_AUTO_CT_SWITCH in config:
        cg.add(
            var.set_auto_ct_switch(
                await cg.get_variable(config[CONF_AUTO_CT_SWITCH])
            )
        )

    # scene buttons
    if CONF_SCENE_COLD_BUTTON in config:
        cg.add(
            var.set_scene_cold_button(
                await cg.get_variable(config[CONF_SCENE_COLD_BUTTON])
            )
        )

    if CONF_SCENE_NEUTRAL_BUTTON in config:
        cg.add(
            var.set_scene_neutral_button(
                await cg.get_variable(config[CONF_SCENE_NEUTRAL_BUTTON])
            )
        )

    if CONF_SCENE_WARM_BUTTON in config:
        cg.add(
            var.set_scene_warm_button(
                await cg.get_variable(config[CONF_SCENE_WARM_BUTTON])
            )
        )

    if CONF_SCENE_NEXT_BUTTON in config:
        cg.add(
            var.set_scene_next_button(
                await cg.get_variable(config[CONF_SCENE_NEXT_BUTTON])
            )
        )

    # effect buttons
    if CONF_EFFECT_FIREPLACE_BUTTON in config:
        cg.add(
            var.set_effect_fireplace_button(
                await cg.get_variable(config[CONF_EFFECT_FIREPLACE_BUTTON])
            )
        )

    if CONF_EFFECT_ALARM_BUTTON in config:
        cg.add(
            var.set_effect_alarm_button(
                await cg.get_variable(config[CONF_EFFECT_ALARM_BUTTON])
            )
        )

    if CONF_EFFECT_STOP_BUTTON in config:
        cg.add(
            var.set_effect_stop_button(
                await cg.get_variable(config[CONF_EFFECT_STOP_BUTTON])
            )
        )
