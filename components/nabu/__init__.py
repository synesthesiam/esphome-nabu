"""Nabu Platform Setup."""

import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID
from esphome import pins

# from esphome.core import coroutine_with_priority

CODEOWNERS = ["@synesthesiam"]
DEPENDENCIES = []

CONF_I2S_DOUT_PIN = "i2s_dout_pin"
CONF_I2S_DIN_PIN = "i2s_din_pin"
CONF_I2S_MCLK_PIN = "i2s_mclk_pin"
CONF_I2S_BCLK_PIN = "i2s_bclk_pin"
CONF_I2S_LRCLK_PIN = "i2s_lrclk_pin"

nabu_ns = cg.esphome_ns.namespace("nabu")
NabuComponent = nabu_ns.class_("NabuComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(NabuComponent),
        cv.Required(CONF_I2S_LRCLK_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_I2S_BCLK_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_I2S_MCLK_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_I2S_DOUT_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_I2S_DIN_PIN): pins.internal_gpio_input_pin_number,
    }
)


# @coroutine_with_priority(55.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_lrclk_pin(config[CONF_I2S_LRCLK_PIN]))
    cg.add(var.set_bclk_pin(config[CONF_I2S_BCLK_PIN]))
    cg.add(var.set_mclk_pin(config[CONF_I2S_MCLK_PIN]))
    cg.add(var.set_dout_pin(config[CONF_I2S_DOUT_PIN]))
    cg.add(var.set_din_pin(config[CONF_I2S_DIN_PIN]))
