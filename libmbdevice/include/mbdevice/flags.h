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

#pragma once

#ifdef __cplusplus

namespace mb
{
namespace device
{

constexpr char ARCH_ARMEABI_V7A[]   = "armeabi-v7a";
constexpr char ARCH_ARM64_V8A[]     = "arm64-v8a";
constexpr char ARCH_X86[]           = "x86";
constexpr char ARCH_X86_64[]        = "x86_64";

enum DeviceFlag : uint32_t
{
    HAS_COMBINED_BOOT_AND_RECOVERY  = 1 << 0,
    FSTAB_SKIP_SDCARD0              = 1 << 1,
};

constexpr uint32_t DEVICE_FLAG_MASK = (1 << 2) - 1;

enum TwFlag : uint32_t
{
    TW_TOUCHSCREEN_SWAP_XY              = 1 << 0,
    TW_TOUCHSCREEN_FLIP_X               = 1 << 1,
    TW_TOUCHSCREEN_FLIP_Y               = 1 << 2,
    TW_GRAPHICS_FORCE_USE_LINELENGTH    = 1 << 3,
    TW_SCREEN_BLANK_ON_BOOT             = 1 << 4,
    TW_BOARD_HAS_FLIPPED_SCREEN         = 1 << 5,
    TW_IGNORE_MAJOR_AXIS_0              = 1 << 6,
    TW_IGNORE_MT_POSITION_0             = 1 << 7,
    TW_IGNORE_ABS_MT_TRACKING_ID        = 1 << 8,
    TW_NEW_ION_HEAP                     = 1 << 9,
    TW_NO_SCREEN_BLANK                  = 1 << 10,
    TW_NO_SCREEN_TIMEOUT                = 1 << 11,
    TW_ROUND_SCREEN                     = 1 << 12,
    TW_NO_CPU_TEMP                      = 1 << 13,
    TW_QCOM_RTC_FIX                     = 1 << 14,
    TW_HAS_DOWNLOAD_MODE                = 1 << 15,
    TW_PREFER_LCD_BACKLIGHT             = 1 << 16,
};

constexpr uint32_t TW_FLAG_MASK = (1 << 17) - 1;

enum class TwPixelFormat
{
    DEFAULT,
    ABGR_8888,
    RGBX_8888,
    BGRA_8888,
    RGBA_8888
};

enum class TwForcePixelFormat
{
    NONE,
    RGB_565
};

constexpr uint64_t VALIDATE_MISSING_ID                        = 1ull << 0;
constexpr uint64_t VALIDATE_MISSING_CODENAMES                 = 1ull << 1;
constexpr uint64_t VALIDATE_MISSING_NAME                      = 1ull << 2;
constexpr uint64_t VALIDATE_MISSING_ARCHITECTURE              = 1ull << 3;
constexpr uint64_t VALIDATE_MISSING_SYSTEM_BLOCK_DEVS         = 1ull << 4;
constexpr uint64_t VALIDATE_MISSING_CACHE_BLOCK_DEVS          = 1ull << 5;
constexpr uint64_t VALIDATE_MISSING_DATA_BLOCK_DEVS           = 1ull << 6;
constexpr uint64_t VALIDATE_MISSING_BOOT_BLOCK_DEVS           = 1ull << 7;
constexpr uint64_t VALIDATE_MISSING_RECOVERY_BLOCK_DEVS       = 1ull << 8;
constexpr uint64_t VALIDATE_MISSING_BOOT_UI_THEME             = 1ull << 9;
constexpr uint64_t VALIDATE_MISSING_BOOT_UI_GRAPHICS_BACKENDS = 1ull << 10;
constexpr uint64_t VALIDATE_INVALID_ARCHITECTURE              = 1ull << 11;
constexpr uint64_t VALIDATE_INVALID_FLAGS                     = 1ull << 12;
constexpr uint64_t VALIDATE_INVALID_BOOT_UI_FLAGS             = 1ull << 13;

}
}

#endif
