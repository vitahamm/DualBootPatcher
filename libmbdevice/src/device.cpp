/*
 * Copyright (C) 2014-2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
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

#include "mbdevice/device.h"


namespace mb
{
namespace device
{

class BaseOptions
{
public:
    std::string id;
    std::vector<std::string> codenames;
    std::string name;
    std::string architecture;
    uint64_t flags;

    std::vector<std::string> base_dirs;
    std::vector<std::string> system_devs;
    std::vector<std::string> cache_devs;
    std::vector<std::string> data_devs;
    std::vector<std::string> boot_devs;
    std::vector<std::string> recovery_devs;
    std::vector<std::string> extra_devs;

    bool operator==(const BaseOptions &other) const;
};

class TwOptions
{
public:
    bool supported;

    uint64_t flags;

    TwPixelFormat pixel_format;
    TwForcePixelFormat force_pixel_format;

    int overscan_percent;
    int default_x_offset;
    int default_y_offset;

    std::string brightness_path;
    std::string secondary_brightness_path;
    int max_brightness;
    int default_brightness;

    std::string battery_path;
    std::string cpu_temp_path;

    std::string input_blacklist;
    std::string input_whitelist;

    std::vector<std::string> graphics_backends;

    std::string theme;

    bool operator==(const TwOptions &other) const;
};

class Device::Impl
{
public:
    BaseOptions options;
    TwOptions tw_options;
};

Device::Device() : m_impl(new Impl())
{
    m_impl->tw_options.pixel_format = TwPixelFormat::DEFAULT;
    m_impl->tw_options.force_pixel_format = TwForcePixelFormat::NONE;
    m_impl->tw_options.max_brightness = -1;
    m_impl->tw_options.default_brightness = -1;
}

Device::Device(const Device &device) : m_impl(new Impl(*device.m_impl))
{
}

Device::Device(Device &&device) : m_impl(std::move(device.m_impl))
{
}

Device::~Device()
{
}

void Device::operator=(const Device &device)
{
    *m_impl = *device.m_impl;
}

void Device::operator=(Device &&device)
{
    m_impl = std::move(device.m_impl);
}

/*!
 * \brief Get the device ID
 *
 * \return Device ID
 */
std::string Device::id() const
{
    return m_impl->options.id;
}

/*!
 * \brief Set the device ID
 *
 * \param id Device ID
 */
void Device::set_id(std::string id)
{
    m_impl->options.id = std::move(id);
}

/*!
 * \brief Get the device codenames
 *
 * \return List of device names
 */
std::vector<std::string> Device::codenames() const
{
    return m_impl->options.codenames;
}

/*!
 * \brief Set the device codenames
 *
 * \param codenames List of device codenames
 */
void Device::set_codenames(std::vector<std::string> codenames)
{
    m_impl->options.codenames = std::move(codenames);
}

/*!
 * \brief Get the device name
 *
 * \return Device name
 */
std::string Device::name() const
{
    return m_impl->options.name;
}

/*!
 * \brief Set the device name
 *
 * \param name Device name
 */
void Device::set_name(std::string name)
{
    m_impl->options.name = std::move(name);
}

/*!
 * \brief Get the device architecture
 *
 * \return Device architecture
 */
std::string Device::architecture() const
{
    return m_impl->options.architecture;
}

/*!
 * \brief Set the device architecture
 *
 * \param architecture Device architecture
 */
void Device::set_architecture(std::string architecture)
{
    m_impl->options.architecture = std::move(architecture);
}

uint64_t Device::flags() const
{
    return m_impl->options.flags;
}

void Device::set_flags(uint64_t flags)
{
    m_impl->options.flags = flags;
}

/*!
 * \brief Get the block device base directories
 *
 * \return List of block device base directories
 */
std::vector<std::string> Device::block_dev_base_dirs() const
{
    return m_impl->options.base_dirs;
}

/*!
 * \brief Set the block device base directories
 *
 * \param base_dirs List of block device base directories
 */
void Device::set_block_dev_base_dirs(std::vector<std::string> base_dirs)
{
    m_impl->options.base_dirs = std::move(base_dirs);
}

/*!
 * \brief Get the system block device paths
 *
 * \return List of system block device paths
 */
std::vector<std::string> Device::system_block_devs() const
{
    return m_impl->options.system_devs;
}

/*!
 * \brief Set the system block device paths
 *
 * \param block_devs List of system block device paths
 */
void Device::set_system_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.system_devs = std::move(block_devs);
}

/*!
 * \brief Get the cache block device paths
 *
 * \return List of cache block device paths
 */
std::vector<std::string> Device::cache_block_devs() const
{
    return m_impl->options.cache_devs;
}

/*!
 * \brief Set the cache block device paths
 *
 * \param block_devs List of cache block device paths
 */
void Device::set_cache_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.cache_devs = std::move(block_devs);
}

/*!
 * \brief Get the data block device paths
 *
 * \return List of data block device paths
 */
std::vector<std::string> Device::data_block_devs() const
{
    return m_impl->options.data_devs;
}

/*!
 * \brief Set the data block device paths
 *
 * \param block_devs List of data block device paths
 */
void Device::set_data_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.data_devs = std::move(block_devs);
}

/*!
 * \brief Get the boot block device paths
 *
 * \return List of boot block device paths
 */
std::vector<std::string> Device::boot_block_devs() const
{
    return m_impl->options.boot_devs;
}

/*!
 * \brief Set the boot block device paths
 *
 * \param block_devs List of boot block device paths
 */
void Device::set_boot_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.boot_devs = std::move(block_devs);
}

/*!
 * \brief Get the recovery block device paths
 *
 * \return List of recovery block devices
 */
std::vector<std::string> Device::recovery_block_devs() const
{
    return m_impl->options.recovery_devs;
}

/*!
 * \brief Set the recovery block device paths
 *
 * \param block_devs List of recovery block device paths
 */
void Device::set_recovery_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.recovery_devs = std::move(block_devs);
}

/*!
 * \brief Get the extra block device paths
 *
 * \return List of extra block device paths
 */
std::vector<std::string> Device::extra_block_devs() const
{
    return m_impl->options.extra_devs;
}

/*!
 * \brief Set the extra block device paths
 *
 * \param block_devs List of extra block device paths
 */
void Device::set_extra_block_devs(std::vector<std::string> block_devs)
{
    m_impl->options.extra_devs = std::move(block_devs);
}

bool Device::tw_supported() const
{
    return m_impl->tw_options.supported;
}

void Device::set_tw_supported(bool supported)
{
    m_impl->tw_options.supported = supported;
}

uint64_t Device::tw_flags() const
{
    return m_impl->tw_options.flags;
}

void Device::set_tw_flags(uint64_t flags)
{
    m_impl->tw_options.flags = flags;
}

TwPixelFormat Device::tw_pixel_format() const
{
    return m_impl->tw_options.pixel_format;
}

void Device::set_tw_pixel_format(TwPixelFormat format)
{
    m_impl->tw_options.pixel_format = format;
}

TwForcePixelFormat Device::tw_force_pixel_format() const
{
    return m_impl->tw_options.force_pixel_format;
}

void Device::set_tw_force_pixel_format(TwForcePixelFormat format)
{
    m_impl->tw_options.force_pixel_format = format;
}

int Device::tw_overscan_percent() const
{
    return m_impl->tw_options.overscan_percent;
}

void Device::set_tw_overscan_percent(int percent)
{
    m_impl->tw_options.overscan_percent = percent;
}

int Device::tw_default_x_offset() const
{
    return m_impl->tw_options.default_x_offset;
}

void Device::set_tw_default_x_offset(int offset)
{
    m_impl->tw_options.default_x_offset = offset;
}

int Device::tw_default_y_offset() const
{
    return m_impl->tw_options.default_y_offset;
}

void Device::set_tw_default_y_offset(int offset)
{
    m_impl->tw_options.default_y_offset = offset;
}

std::string Device::tw_brightness_path() const
{
    return m_impl->tw_options.brightness_path;
}

void Device::set_tw_brightness_path(std::string path)
{
    m_impl->tw_options.brightness_path = std::move(path);
}

std::string Device::tw_secondary_brightness_path() const
{
    return m_impl->tw_options.secondary_brightness_path;
}

void Device::set_tw_secondary_brightness_path(std::string path)
{
    m_impl->tw_options.secondary_brightness_path = std::move(path);
}

int Device::tw_max_brightness() const
{
    return m_impl->tw_options.max_brightness;
}

void Device::set_tw_max_brightness(int value)
{
    m_impl->tw_options.max_brightness = value;
}

int Device::tw_default_brightness() const
{
    return m_impl->tw_options.default_brightness;
}

void Device::set_tw_default_brightness(int value)
{
    m_impl->tw_options.default_brightness = value;
}

std::string Device::tw_battery_path() const
{
    return m_impl->tw_options.battery_path;
}

void Device::set_tw_battery_path(std::string path)
{
    m_impl->tw_options.battery_path = std::move(path);
}

std::string Device::tw_cpu_temp_path() const
{
    return m_impl->tw_options.cpu_temp_path;
}

void Device::set_tw_cpu_temp_path(std::string path)
{
    m_impl->tw_options.cpu_temp_path = std::move(path);
}

std::string Device::tw_input_blacklist() const
{
    return m_impl->tw_options.input_blacklist;
}

void Device::set_tw_input_blacklist(std::string blacklist)
{
    m_impl->tw_options.input_blacklist = std::move(blacklist);
}

std::string Device::tw_input_whitelist() const
{
    return m_impl->tw_options.input_whitelist;
}

void Device::set_tw_input_whitelist(std::string whitelist)
{
    m_impl->tw_options.input_whitelist = std::move(whitelist);
}

std::vector<std::string> Device::tw_graphics_backends() const
{
    return m_impl->tw_options.graphics_backends;
}

void Device::set_tw_graphics_backends(std::vector<std::string> backends)
{
    m_impl->tw_options.graphics_backends = std::move(backends);
}

std::string Device::tw_theme() const
{
    return m_impl->tw_options.theme;
}

void Device::set_tw_theme(std::string theme)
{
    m_impl->tw_options.theme = std::move(theme);
}

uint64_t Device::validate() const
{
    uint64_t flags = 0;

    if (m_impl->options.id.empty()) {
        flags |= VALIDATE_MISSING_ID;
    }

    if (m_impl->options.codenames.empty()) {
        flags |= VALIDATE_MISSING_CODENAMES;
    }

    if (m_impl->options.name.empty()) {
        flags |= VALIDATE_MISSING_NAME;
    }

    if (m_impl->options.architecture.empty()) {
        flags |= VALIDATE_MISSING_ARCHITECTURE;
    } else if (m_impl->options.architecture != ARCH_ARMEABI_V7A
            && m_impl->options.architecture != ARCH_ARM64_V8A
            && m_impl->options.architecture != ARCH_X86
            && m_impl->options.architecture != ARCH_X86_64) {
        flags |= VALIDATE_INVALID_ARCHITECTURE;
    }

    if ((m_impl->options.flags & DEVICE_FLAG_MASK) != m_impl->options.flags) {
        flags |= VALIDATE_INVALID_FLAGS;
    }

    if (m_impl->options.system_devs.empty()) {
        flags |= VALIDATE_MISSING_SYSTEM_BLOCK_DEVS;
    }

    if (m_impl->options.cache_devs.empty()) {
        flags |= VALIDATE_MISSING_CACHE_BLOCK_DEVS;
    }

    if (m_impl->options.data_devs.empty()) {
        flags |= VALIDATE_MISSING_DATA_BLOCK_DEVS;
    }

    if (m_impl->options.boot_devs.empty()) {
        flags |= VALIDATE_MISSING_BOOT_BLOCK_DEVS;
    }

    //if (m_impl->options.recovery_devs.empty()) {
    //    flags |= VALIDATE_MISSING_RECOVERY_BLOCK_DEVS;
    //}

    if (m_impl->tw_options.supported) {
        if ((m_impl->tw_options.flags & TW_FLAG_MASK)
                != m_impl->tw_options.flags) {
            flags |= VALIDATE_INVALID_BOOT_UI_FLAGS;
        }

        if (m_impl->tw_options.theme.empty()) {
            flags |= VALIDATE_MISSING_BOOT_UI_THEME;
        }

        if (m_impl->tw_options.graphics_backends.empty()) {
            flags |= VALIDATE_MISSING_BOOT_UI_GRAPHICS_BACKENDS;
        }
    }

    return flags;
}

bool BaseOptions::operator==(const BaseOptions &other) const
{
    return id == other.id
            && codenames == other.codenames
            && name == other.name
            && architecture == other.architecture
            && flags == other.flags
            && base_dirs == other.base_dirs
            && system_devs == other.system_devs
            && cache_devs == other.cache_devs
            && data_devs == other.data_devs
            && boot_devs == other.boot_devs
            && recovery_devs == other.recovery_devs
            && extra_devs == other.extra_devs;
}

bool TwOptions::operator==(const TwOptions &other) const
{
    return supported == other.supported
            && flags == other.flags
            && pixel_format == other.pixel_format
            && force_pixel_format == other.force_pixel_format
            && overscan_percent == other.overscan_percent
            && default_x_offset == other.default_x_offset
            && default_y_offset == other.default_y_offset
            && brightness_path == other.brightness_path
            && secondary_brightness_path == other.secondary_brightness_path
            && max_brightness == other.max_brightness
            && default_brightness == other.default_brightness
            && battery_path == other.battery_path
            && cpu_temp_path == other.cpu_temp_path
            && input_blacklist == other.input_blacklist
            && input_whitelist == other.input_whitelist
            && graphics_backends == other.graphics_backends
            && theme == other.theme;
}

bool Device::operator==(const Device &other) const
{
    return m_impl->options == other.m_impl->options
            && m_impl->tw_options == other.m_impl->tw_options;
}

}
}
