/*
 * Copyright (C) 2016-2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * This file is part of MultiBootPatcher
 *
 * MultiBootPatcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MultiBootPatcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MultiBootPatcher.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mbdevice/json.h"

#include <json/json.h>
#include <json/writer.h>

#include "mbcommon/string.h"


#define JSON_BOOLEAN JSON_TRUE

namespace mb
{
namespace device
{

struct FlagMapping
{
    const char *key;
    uint32_t flag;
};

static struct FlagMapping device_flag_mappings[] = {
#define FLAG(F) { #F, DeviceFlag::F }
    FLAG(HAS_COMBINED_BOOT_AND_RECOVERY),
    FLAG(FSTAB_SKIP_SDCARD0),
#undef FLAG
    { nullptr, 0 }
};

static struct FlagMapping tw_flag_mappings[] = {
#define FLAG(F) { #F, TwFlag::F }
    FLAG(TW_TOUCHSCREEN_SWAP_XY),
    FLAG(TW_TOUCHSCREEN_FLIP_X),
    FLAG(TW_TOUCHSCREEN_FLIP_Y),
    FLAG(TW_GRAPHICS_FORCE_USE_LINELENGTH),
    FLAG(TW_SCREEN_BLANK_ON_BOOT),
    FLAG(TW_BOARD_HAS_FLIPPED_SCREEN),
    FLAG(TW_IGNORE_MAJOR_AXIS_0),
    FLAG(TW_IGNORE_MT_POSITION_0),
    FLAG(TW_IGNORE_ABS_MT_TRACKING_ID),
    FLAG(TW_NEW_ION_HEAP),
    FLAG(TW_NO_SCREEN_BLANK),
    FLAG(TW_NO_SCREEN_TIMEOUT),
    FLAG(TW_ROUND_SCREEN),
    FLAG(TW_NO_CPU_TEMP),
    FLAG(TW_QCOM_RTC_FIX),
    FLAG(TW_HAS_DOWNLOAD_MODE),
    FLAG(TW_PREFER_LCD_BACKLIGHT),
#undef FLAG
    { nullptr, 0 }
};

struct TwPixelFormatMapping
{
    const char *key;
    TwPixelFormat value;
};

static struct TwPixelFormatMapping tw_pxfmt_mappings[] = {
#define FLAG(F) { #F, TwPixelFormat::F }
    FLAG(DEFAULT),
    FLAG(ABGR_8888),
    FLAG(RGBX_8888),
    FLAG(BGRA_8888),
    FLAG(RGBA_8888),
#undef FLAG
    { nullptr, TwPixelFormat::DEFAULT }
};

struct TwForcePixelFormatMapping
{
    const char *key;
    TwForcePixelFormat value;
};

static struct TwForcePixelFormatMapping tw_force_pxfmt_mappings[] = {
#define FLAG(F) { #F, TwForcePixelFormat::F }
    FLAG(NONE),
    FLAG(RGB_565),
#undef FLAG
    { NULL, TwForcePixelFormat::NONE }
};

static std::string json_type_to_string(Json::ValueType type)
{
    switch (type) {
    case Json::nullValue:
        return "null";
    case Json::intValue:
        return "integer";
    case Json::uintValue:
        return "uinteger";
    case Json::realValue:
        return "real";
    case Json::stringValue:
        return "string";
    case Json::booleanValue:
        return "boolean";
    case Json::arrayValue:
        return "array";
    case Json::objectValue:
        return "object";
    default:
        return {};
    }
}

static void json_error_set_parse_error(JsonError &error,
                                       std::string parser_msg)
{
    error.type = JsonErrorType::PARSE_ERROR;
    error.parser_msg = std::move(parser_msg);
}

static void json_error_set_mismatched_type(JsonError &error,
                                           std::string context,
                                           Json::ValueType actual_type,
                                           Json::ValueType expected_type)
{
    error.type = JsonErrorType::MISMATCHED_TYPE;

    if (context.empty()) {
        error.context = ".";
    } else {
        error.context = std::move(context);
    }

    error.actual_type = json_type_to_string(actual_type);
    error.expected_type = json_type_to_string(expected_type);
}

static void json_error_set_unknown_key(JsonError &error,
                                       std::string context)
{
    error.type = JsonErrorType::UNKNOWN_KEY;

    if (context.empty()) {
        error.context = ".";
    } else {
        error.context = std::move(context);
    }
}

static void json_error_set_unknown_value(JsonError &error,
                                         std::string context)
{
    error.type = JsonErrorType::UNKNOWN_VALUE;

    if (context.empty()) {
        error.context = ".";
    } else {
        error.context = std::move(context);
    }
}

static inline bool device_set_boolean(void (Device::*setter)(bool),
                                      Device &device, const Json::Value &node,
                                      const std::string &context, JsonError &error)
{
    if (!node.isBool()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::booleanValue);
        return false;
    }

    (device.*setter)(node.asBool());
    return true;
}

static inline bool device_set_int(void (Device::*setter)(int),
                                  Device &device, const Json::Value &node,
                                  const std::string &context, JsonError &error)
{
    if (!node.isInt()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::intValue);
        return false;
    }

    (device.*setter)(node.asInt());
    return true;
}

static inline bool device_set_string(void (Device::*setter)(std::string),
                                     Device &device, const Json::Value &node,
                                     const std::string &context, JsonError &error)
{
    if (!node.isString()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::stringValue);
        return false;
    }

    (device.*setter)(node.asString());
    return true;
}

static inline bool device_set_string_array(void (Device::*setter)(std::vector<std::string>),
                                           Device &device, const Json::Value &node,
                                           const std::string &context, JsonError &error)
{
    std::string subcontext;
    size_t index = 0;
    std::vector<std::string> array;

    if (!node.isArray()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::arrayValue);
        return false;
    }

    for (auto const &item : node) {
#ifdef __ANDROID__
        subcontext = mb::format("%s[%" MB_PRIzu "]", context.c_str(), index);
#else
        subcontext = context;
        subcontext += "[";
        subcontext += std::to_string(index);
        subcontext += "]";
#endif
        ++index;

        if (!item.isString()) {
            json_error_set_mismatched_type(
                    error, subcontext, item.type(), Json::stringValue);
            return false;
        }

        array.push_back(item.asString());
    }

    (device.*setter)(std::move(array));
    return true;
}

static bool process_device_flags(Device &device, const Json::Value &node,
                                 const std::string &context, JsonError &error)
{
    std::string subcontext;
    size_t index = 0;
    uint64_t flags = 0;

    if (!node.isArray()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::arrayValue);
        return false;
    }

    for (auto const &item : node) {
#ifdef __ANDROID__
        subcontext = mb::format("%s[%" MB_PRIzu "]", context.c_str(), index);
#else
        subcontext = context;
        subcontext += "[";
        subcontext += std::to_string(index);
        subcontext += "]";
#endif
        ++index;

        if (!item.isString()) {
            json_error_set_mismatched_type(
                    error, subcontext, item.type(), Json::stringValue);
            return false;
        }

        const std::string &str = item.asString();
        uint64_t old_flags = flags;

        for (auto it = device_flag_mappings; it->key; ++it) {
            if (str == it->key) {
                flags |= it->flag;
                break;
            }
        }

        if (flags == old_flags) {
            json_error_set_unknown_value(error, subcontext);
            return false;
        }
    }

    device.set_flags(flags);
    return true;
}

static bool process_boot_ui_flags(Device &device, const Json::Value &node,
                                  const std::string &context, JsonError &error)
{
    std::string subcontext;
    size_t index = 0;
    uint64_t flags = 0;

    if (!node.isArray()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::arrayValue);
        return false;
    }

    for (auto const &item : node) {
#ifdef __ANDROID__
        subcontext = mb::format("%s[%" MB_PRIzu "]", context.c_str(), index);
#else
        subcontext = context;
        subcontext += "[";
        subcontext += std::to_string(index);
        subcontext += "]";
#endif
        ++index;

        if (!item.isString()) {
            json_error_set_mismatched_type(
                    error, subcontext, item.type(), Json::stringValue);
            return false;
        }

        const std::string &str = item.asString();
        uint64_t old_flags = flags;

        for (auto it = tw_flag_mappings; it->key; ++it) {
            if (str == it->key) {
                flags |= it->flag;
                break;
            }
        }

        if (flags == old_flags) {
            json_error_set_unknown_value(error, subcontext);
            return false;
        }
    }

    device.set_tw_flags(flags);
    return true;
}

static bool process_boot_ui_pixel_format(Device &device,
                                         const Json::Value &node,
                                         const std::string &context,
                                         JsonError &error)
{
    std::string str;

    if (!node.isString()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::stringValue);
        return false;
    }

    str = node.asString();

    for (auto it = tw_pxfmt_mappings; it->key; ++it) {
        if (str == it->key) {
            device.set_tw_pixel_format(it->value);
            return true;
        }
    }

    json_error_set_unknown_value(error, context);
    return false;
}

static bool process_boot_ui_force_pixel_format(Device &device,
                                               const Json::Value &node,
                                               const std::string &context,
                                               JsonError &error)
{
    std::string str;

    if (!node.isString()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::stringValue);
        return false;
    }

    str = node.asString();

    for (auto it = tw_force_pxfmt_mappings; it->key; ++it) {
        if (str == it->key) {
            device.set_tw_force_pixel_format(it->value);
            return true;
        }
    }

    json_error_set_unknown_value(error, context);
    return false;
}

static bool process_boot_ui(Device &device, const Json::Value &node,
                            const std::string &context, JsonError &error)
{
    bool ret;
    std::string subcontext;

    if (!node.isObject()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::objectValue);
        return false;
    }

    for (auto it = node.begin(); it != node.end(); ++it) {
        auto const &key = it.key().asString();

        subcontext = context;
        subcontext += ".";
        subcontext += key;

        if (key == "supported") {
            ret = device_set_boolean(&Device::set_tw_supported,
                                     device, *it, subcontext, error);
        } else if (key == "flags") {
            ret = process_boot_ui_flags(device, *it, subcontext, error);
        } else if (key == "pixel_format") {
            ret = process_boot_ui_pixel_format(device, *it, subcontext, error);
        } else if (key == "force_pixel_format") {
            ret = process_boot_ui_force_pixel_format(device, *it, subcontext, error);
        } else if (key == "overscan_percent") {
            ret = device_set_int(&Device::set_tw_overscan_percent,
                                 device, *it, subcontext, error);
        } else if (key == "default_x_offset") {
            ret = device_set_int(&Device::set_tw_default_x_offset,
                                 device, *it, subcontext, error);
        } else if (key == "default_y_offset") {
            ret = device_set_int(&Device::set_tw_default_y_offset,
                                 device, *it, subcontext, error);
        } else if (key == "brightness_path") {
            ret = device_set_string(&Device::set_tw_brightness_path,
                                    device, *it, subcontext, error);
        } else if (key == "secondary_brightness_path") {
            ret = device_set_string(&Device::set_tw_secondary_brightness_path,
                                    device, *it, subcontext, error);
        } else if (key == "max_brightness") {
            ret = device_set_int(&Device::set_tw_max_brightness,
                                 device, *it, subcontext, error);
        } else if (key == "default_brightness") {
            ret = device_set_int(&Device::set_tw_default_brightness,
                                 device, *it, subcontext, error);
        } else if (key == "battery_path") {
            ret = device_set_string(&Device::set_tw_battery_path,
                                    device, *it, subcontext, error);
        } else if (key == "cpu_temp_path") {
            ret = device_set_string(&Device::set_tw_cpu_temp_path,
                                    device, *it, subcontext, error);
        } else if (key == "input_blacklist") {
            ret = device_set_string(&Device::set_tw_input_blacklist,
                                    device, *it, subcontext, error);
        } else if (key == "input_whitelist") {
            ret = device_set_string(&Device::set_tw_input_whitelist,
                                    device, *it, subcontext, error);
        } else if (key == "graphics_backends") {
            ret = device_set_string_array(&Device::set_tw_graphics_backends,
                                          device, *it, subcontext, error);
        } else if (key == "theme") {
            ret = device_set_string(&Device::set_tw_theme,
                                    device, *it, subcontext, error);
        } else {
            json_error_set_unknown_key(error, subcontext);
            ret = false;
        }

        if (!ret) {
            break;
        }
    }

    return ret;
}


static bool process_block_devs(Device &device, const Json::Value &node,
                               const std::string &context, JsonError &error)
{
    bool ret;
    std::string subcontext;

    if (!node.isObject()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::objectValue);
        return false;
    }

    for (auto it = node.begin(); it != node.end(); ++it) {
        auto const &key = it.key().asString();

        subcontext = context;
        subcontext += ".";
        subcontext += key;

        if (key == "base_dirs") {
            ret = device_set_string_array(&Device::set_block_dev_base_dirs,
                                          device, *it, subcontext, error);
        } else if (key == "system") {
            ret = device_set_string_array(&Device::set_system_block_devs,
                                          device, *it, subcontext, error);
        } else if (key == "cache") {
            ret = device_set_string_array(&Device::set_cache_block_devs,
                                          device, *it, subcontext, error);
        } else if (key == "data") {
            ret = device_set_string_array(&Device::set_data_block_devs,
                                          device, *it, subcontext, error);
        } else if (key == "boot") {
            ret = device_set_string_array(&Device::set_boot_block_devs,
                                          device, *it, subcontext, error);
        } else if (key == "recovery") {
            ret = device_set_string_array(&Device::set_recovery_block_devs,
                                          device, *it, subcontext, error);
        } else if (key == "extra") {
            ret = device_set_string_array(&Device::set_extra_block_devs,
                                          device, *it, subcontext, error);
        } else {
            json_error_set_unknown_key(error, subcontext);
            ret = false;
        }

        if (!ret) {
            break;
        }
    }

    return ret;
}

static bool process_device(Device &device, const Json::Value &node,
                           const std::string &context, JsonError &error)
{
    bool ret;
    std::string subcontext;

    if (!node.isObject()) {
        json_error_set_mismatched_type(
                error, context, node.type(), Json::objectValue);
        return false;
    }

    for (auto it = node.begin(); it != node.end(); ++it) {
        auto const &key = it.key().asString();

        subcontext = context;
        subcontext += ".";
        subcontext += key;

        if (key == "name") {
            ret = device_set_string(&Device::set_name,
                                    device, *it, subcontext, error);
        } else if (key == "id") {
            ret = device_set_string(&Device::set_id,
                                    device, *it, subcontext, error);
        } else if (key == "codenames") {
            ret = device_set_string_array(&Device::set_codenames,
                                          device, *it, subcontext, error);
        } else if (key == "architecture") {
            ret = device_set_string(&Device::set_architecture,
                                    device, *it, subcontext, error);
        } else if (key == "flags") {
            ret = process_device_flags(device, *it, subcontext, error);
        } else if (key == "block_devs") {
            ret = process_block_devs(device, *it, subcontext, error);
        } else if (key == "boot_ui") {
            ret = process_boot_ui(device, *it, subcontext, error);
        } else {
            json_error_set_unknown_key(error, subcontext);
            ret = false;
        }

        if (!ret) {
            break;
        }
    }

    return ret;
}

bool device_from_json(const std::string &json, Device &device, JsonError &error)
{
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(json, root, false)) {
        json_error_set_parse_error(error, reader.getFormattedErrorMessages());
        return false;
    }

    return process_device(device, root, "", error);
}

bool device_list_from_json(const std::string &json,
                           std::vector<Device> &devices,
                           JsonError &error)
{
    Json::Value root;
    Json::Reader reader;
    size_t index = 0;
    std::string context;

    if (!reader.parse(json, root, false)) {
        json_error_set_parse_error(error, reader.getFormattedErrorMessages());
        return false;
    }

    if (!root.isArray()) {
        json_error_set_mismatched_type(
                error, "", root.type(), Json::arrayValue);
        return false;
    }

    for (auto const &item : root) {
#ifdef __ANDROID__
        context = mb::format("[%" MB_PRIzu "]", index);
#else
        context = "[";
        context += std::to_string(index);
        context += "]";
#endif
        ++index;

        Device device;

        if (!process_device(device, item, context, error)) {
            return false;
        }

        devices.push_back(std::move(device));
    }

    return true;
}

std::string device_to_json(const Device &device)
{
    Json::Value root;
    Json::Value block_devs;
    Json::Value boot_ui;

    auto const &id = device.id();
    if (!id.empty()) {
        root["id"] = id;
    }

    for (auto const &codename : device.codenames()) {
        root["codenames"].append(codename);
    }

    auto const &name = device.name();
    if (!name.empty()) {
        root["name"] = name;
    }

    auto const &architecture = device.architecture();
    if (!architecture.empty()) {
        root["architecture"] = architecture;
    }

    auto const flags = device.flags();
    if (flags != 0) {
        for (auto it = device_flag_mappings; it->key; ++it) {
            if (flags & it->flag) {
                root["flags"].append(it->key);
            }
        }
    }

    /* Block devs */

    for (auto const &base_dir : device.block_dev_base_dirs()) {
        block_devs["base_dirs"].append(base_dir);
    }

    for (auto const &block_dev : device.system_block_devs()) {
        block_devs["system"].append(block_dev);
    }

    for (auto const &block_dev : device.cache_block_devs()) {
        block_devs["cache"].append(block_dev);
    }

    for (auto const &block_dev : device.data_block_devs()) {
        block_devs["data"].append(block_dev);
    }

    for (auto const &block_dev : device.boot_block_devs()) {
        block_devs["boot"].append(block_dev);
    }

    for (auto const &block_dev : device.recovery_block_devs()) {
        block_devs["recovery"].append(block_dev);
    }

    for (auto const &block_dev : device.extra_block_devs()) {
        block_devs["extra"].append(block_dev);
    }

    if (!block_devs.empty()) {
        root["block_devs"] = std::move(block_devs);
    }

    /* Boot UI */

    if (device.tw_supported()) {
        boot_ui["supported"] = true;
    }

    auto const tw_flags = device.tw_flags();
    if (tw_flags != 0) {
        for (auto it = tw_flag_mappings; it->key; ++it) {
            if (tw_flags & it->flag) {
                boot_ui["flags"].append(it->key);
            }
        }
    }

    auto const pixel_format = device.tw_pixel_format();
    if (pixel_format != TwPixelFormat::DEFAULT) {
        for (auto it = tw_pxfmt_mappings; it->key; ++it) {
            if (pixel_format == it->value) {
                boot_ui["pixel_format"] = it->key;
                break;
            }
        }
    }

    auto const force_pixel_format = device.tw_force_pixel_format();
    if (force_pixel_format != TwForcePixelFormat::NONE) {
        for (auto it = tw_force_pxfmt_mappings; it->key; ++it) {
            if (force_pixel_format == it->value) {
                boot_ui["force_pixel_format"] = it->key;
                break;
            }
        }
    }

    auto const overscan_percent = device.tw_overscan_percent();
    if (overscan_percent != 0) {
        boot_ui["overscan_percent"] = overscan_percent;
    }

    auto const default_x_offset = device.tw_default_x_offset();
    if (default_x_offset != 0) {
        boot_ui["default_x_offset"] = default_x_offset;
    }

    auto const default_y_offset = device.tw_default_y_offset();
    if (default_y_offset != 0) {
        boot_ui["default_y_offset"] = default_y_offset;
    }

    auto const &brightness_path = device.tw_brightness_path();
    if (!brightness_path.empty()) {
        boot_ui["brightness_path"] = brightness_path;
    }

    auto const &secondary_brightness_path =
            device.tw_secondary_brightness_path();
    if (!secondary_brightness_path.empty()) {
        boot_ui["secondary_brightness_path"] = secondary_brightness_path;
    }

    auto const max_brightness = device.tw_max_brightness();
    if (max_brightness != -1) {
        boot_ui["max_brightness"] = max_brightness;
    }

    auto const default_brightness = device.tw_default_brightness();
    if (default_brightness != -1) {
        boot_ui["default_brightness"] = default_brightness;
    }

    auto const &battery_path = device.tw_battery_path();
    if (!battery_path.empty()) {
        boot_ui["battery_path"] = battery_path;
    }

    auto const &cpu_temp_path = device.tw_cpu_temp_path();
    if (!cpu_temp_path.empty()) {
        boot_ui["cpu_temp_path"] = cpu_temp_path;
    }

    auto const &input_blacklist = device.tw_input_blacklist();
    if (!input_blacklist.empty()) {
        boot_ui["input_blacklist"] = input_blacklist;
    }

    auto const &input_whitelist = device.tw_input_whitelist();
    if (!input_whitelist.empty()) {
        boot_ui["input_whitelist"] = input_whitelist;
    }

    for (auto const &backend : device.tw_graphics_backends()) {
        boot_ui["graphics_backends"].append(backend);
    }

    auto const &theme = device.tw_theme();
    if (!theme.empty()) {
        boot_ui["theme"] = theme;
    }

    if (!boot_ui.empty()) {
        root["boot_ui"] = std::move(boot_ui);
    }

    return Json::FastWriter().write(root);
}

}
}
