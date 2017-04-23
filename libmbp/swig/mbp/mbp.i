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

%module mbp

%{
#include "mbp/errors.h"
#include "mbp/fileinfo.h"
#include "mbp/patcherconfig.h"
#include "mbp/patcherinterface.h"
%}

%import "swigcommon/swigcommon.i"
%import "swigcommon/style/java.i"

// Delete move constructor and assignment operator
%ignore mbp::PatcherConfig::PatcherConfig(PatcherConfig &&);
%ignore mbp::PatcherConfig::operator=(PatcherConfig &&);

// For macros
%import "mbcommon/common.h"

// libmbdevice
%import "mbdevice/mbdevice.i"

// libmbp
%include "mbp/errors.h"
%include "mbp/fileinfo.h"
%include "mbp/patcherconfig.h"
%include "mbp/patcherinterface.h"

%pragma(java) jniclasscode=%{
    static {
        System.loadLibrary("mbp-jni");
    }
%}
