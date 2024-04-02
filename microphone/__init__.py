"""Nabu Microphone Setup."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import microphone

from .. import nabu_ns, NabuComponent

CODEOWNERS = ["@synesthesiam"]
DEPENDENCIES = ["nabu", "microphone"]

NabuMicrophone = nabu_ns.class_("NabuMicrophone")
NabuMicrophone = nabu_ns.class_(
    "NabuMicrophone", NabuMicrophone, microphone.Microphone, cg.Component
)

CONF_NABU_ID = "nabu_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(NabuMicrophone),
        cv.GenerateID(CONF_NABU_ID): cv.use_id(NabuComponent),
    }
)

# @coroutine_with_priority(100.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_NABU_ID])
