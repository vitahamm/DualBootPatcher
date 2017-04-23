/*
 * Copyright (C) 2017  Andrew Gunnerson <andrewgunnerson@gmail.com>
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

%module mbdevice

%{
#include "mbdevice/device.h"
#include "mbdevice/flags.h"
#include "mbdevice/json.h"
%}

%import "swigcommon/swigcommon.i"
%import "swigcommon/style/java.i"

// Explicit std::vector instantiations
%template(DeviceVector) std::vector<mb::device::Device>;

// Delete move constructor and assignment operator
%ignore mb::device::Device::Device(Device &&);
%ignore mb::device::Device::operator=(Device &&);

// Rename equality and assignment operators
%rename(assign) mb::device::Device::operator=(const Device &);
%rename(equals) mb::device::Device::operator==(const Device &) const;

// For macros
%import "mbcommon/common.h"

// libmbdevice
%include "mbdevice/flags.h"
%include "mbdevice/device.h"
%include "mbdevice/json.h"

%pragma(java) jniclasscode=%{
    static {
        System.loadLibrary("mbdevice-jni");
    }
%}
