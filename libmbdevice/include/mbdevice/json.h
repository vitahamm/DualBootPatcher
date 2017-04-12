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

#pragma once

#include "mbdevice/device.h"

#ifdef __cplusplus
namespace mb
{
namespace device
{

enum class JsonErrorType
{
    // Use |parser_msg| field
    PARSE_ERROR,
    // Use |context|, |expected_type|, and |actual_type| fields
    MISMATCHED_TYPE,
    // Use |context| field
    UNKNOWN_KEY,
    // Use |context] field
    UNKNOWN_VALUE,
};

struct JsonError
{
    JsonErrorType type;

    std::string parser_msg;
    std::string context;
    std::string expected_type;
    std::string actual_type;
};

MB_EXPORT bool device_from_json(const std::string &json, Device &device,
                                JsonError &error);

MB_EXPORT bool device_list_from_json(const std::string &json,
                                     std::vector<Device> &devices,
                                     JsonError &error);

MB_EXPORT std::string device_to_json(const Device &device);

}
}
#endif
