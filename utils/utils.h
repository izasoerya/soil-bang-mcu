#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <esp_system.h>

String esp_reset_reason_to_string(esp_reset_reason_t reason)
{
    switch (reason)
    {
    case ESP_RST_POWERON:
        return "POWERON_RESET";
    case ESP_RST_EXT:
        return "EXT_RESET";
    case ESP_RST_SW:
        return "SW_RESET";
    case ESP_RST_PANIC:
        return "PANIC_RESET";
    case ESP_RST_INT_WDT:
        return "INT_WATCHDOG_RESET";
    case ESP_RST_TASK_WDT:
        return "TASK_WATCHDOG_RESET";
    case ESP_RST_WDT:
        return "OTHER_WATCHDOG_RESET";
    case ESP_RST_DEEPSLEEP:
        return "DEEPSLEEP_RESET";
    case ESP_RST_BROWNOUT:
        return "BROWNOUT_RESET";
    case ESP_RST_SDIO:
        return "SDIO_RESET";
    default:
        return "UNKNOWN_RESET";
    }
}

#endif