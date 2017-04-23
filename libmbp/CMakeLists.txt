set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${MBP_LIBARCHIVE_INCLUDES})
include_directories(${MBP_LIBLZMA_INCLUDES})
include_directories(${MBP_ZLIB_INCLUDES})

include_directories(${CMAKE_SOURCE_DIR}/external)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(MBP_SOURCES
    src/fileinfo.cpp
    src/patcherconfig.cpp
    # Edify tokenizer
    src/edify/tokenizer.cpp
    # Private classes
    src/private/fileutils.cpp
    src/private/miniziputils.cpp
    src/private/stringutils.cpp
    # Autopatchers
    src/autopatchers/standardpatcher.cpp
    src/autopatchers/mountcmdpatcher.cpp
    # Patchers
    src/patchers/multibootpatcher.cpp
    src/patchers/odinpatcher.cpp
    src/patchers/ramdiskupdater.cpp
    # SHA1 library
    external/sha.cpp
)

set(MBP_SWIG_SOURCE swig/mbp/mbp.i)

if(WIN32)
    list(APPEND MBP_SOURCES
        src/private/win32.cpp
    )
endif()

# minizip type safety
add_definitions(-DSTRICTZIPUNZIP)

if(${MBP_BUILD_TARGET} STREQUAL android-system)
    # Build static library

    add_library(mbp-static STATIC ${MBP_SOURCES})

    target_compile_definitions(
        mbp-static
        PRIVATE
        -DMB_LIBRARY
    )

    if(NOT MSVC)
        set_target_properties(
            mbp-static
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED 1
            C_STANDARD 99
            C_STANDARD_REQUIRED 1
        )
    endif()
elseif(${MBP_BUILD_TARGET} STREQUAL android-app
        OR ${MBP_BUILD_TARGET} STREQUAL desktop)
    # Build shared library

    add_library(mbp-shared SHARED ${MBP_SOURCES})

    target_compile_definitions(
        mbp-shared
        PRIVATE
        -DMB_DYNAMIC_LINK
        -DMB_LIBRARY
    )

    set_target_properties(
        mbp-shared
        PROPERTIES
        POSITION_INDEPENDENT_CODE 1
        OUTPUT_NAME mbp
    )

    if(NOT MSVC)
        set_target_properties(
            mbp-shared
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED 1
            C_STANDARD 99
            C_STANDARD_REQUIRED 1
        )
    endif()

    if(${MBP_BUILD_TARGET} STREQUAL android-app)
        set(ADDITIONAL_LIBRARIES c_compat)
    else()
        set(ADDITIONAL_LIBRARIES)
    endif()

    target_link_libraries(
        mbp-shared
        mbpio-static
        mbdevice-shared
        mblog-shared
        mbcommon-shared
        minizip-shared
        ${ADDITIONAL_LIBRARIES}
        ${MBP_LIBARCHIVE_LIBRARIES}
        ${MBP_LIBLZMA_LIBRARIES}
        ${MBP_ZLIB_LIBRARIES}
    )

    if(UNIX AND NOT ANDROID)
        target_link_libraries(mbp-shared pthread)
    endif()

    # Install library
    install(
        TARGETS mbp-shared
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        RUNTIME DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        #ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
    )
endif()

# swig
if(${MBP_BUILD_TARGET} STREQUAL android-app)
    set_source_files_properties(
        ${MBP_SWIG_SOURCE}
        PROPERTIES
        CPLUSPLUS ON
    )

    swig_add_module(
        mbp-jni
        java
        ${MBP_SWIG_SOURCE}
    )

    swig_link_libraries(
        mbp-jni
        mbp-shared
    )

    if(NOT MSVC)
        set_target_properties(
            mbp-jni
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED 1
        )
    endif()

    target_compile_options(
        mbp-jni
        PRIVATE
        -fexceptions
    )

    install(
        TARGETS mbp-jni
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        RUNTIME DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        #ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
    )
endif()
