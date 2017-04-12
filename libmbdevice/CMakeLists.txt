include_directories(${MBP_JSONCPP_INCLUDES})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(MBDEVICE_SOURCES
    src/device.cpp
    src/json.cpp
)

set(MBDEVICE_TESTS_SOURCES
    # Helpers
    tests/main.cpp
    # Tests
    tests/test_device.cpp
    tests/test_json.cpp
)

set(variants)

if(${MBP_BUILD_TARGET} STREQUAL android-system)
    list(APPEND variants static)
elseif(${MBP_BUILD_TARGET} STREQUAL android-app
        OR ${MBP_BUILD_TARGET} STREQUAL desktop
        OR ${MBP_BUILD_TARGET} STREQUAL hosttools)
    list(APPEND variants shared)
endif()

# Build libraries
foreach(variant ${variants})
    set(lib_target mbdevice-${variant})
    set(obj_target ${lib_target}-obj)

    # Build objects
    add_library(${obj_target} OBJECT ${MBDEVICE_SOURCES})

    # Export symbols
    target_compile_definitions(${obj_target} PRIVATE -DMB_LIBRARY)

    # Win32 DLL export
    if(${variant} STREQUAL shared)
        target_compile_definitions(${obj_target} PRIVATE -DMB_DYNAMIC_LINK)
    endif()

    # Build with -fPIC
    set_target_properties(${obj_target} PROPERTIES POSITION_INDEPENDENT_CODE 1)

    # Target C++11
    if(NOT MSVC)
        set_target_properties(
            ${obj_target}
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED 1
        )
    endif()

    # Build library
    if(${variant} STREQUAL shared)
        add_library(${lib_target} SHARED $<TARGET_OBJECTS:${obj_target}>)
    elseif(${variant} STREQUAL static)
        add_library(${lib_target} STATIC $<TARGET_OBJECTS:${obj_target}>)
    endif()

    # Set library name
    set_target_properties(${lib_target} PROPERTIES OUTPUT_NAME mbdevice)

    # Link dependencies
    if(${variant} STREQUAL shared)
        target_link_libraries(
            ${lib_target}
            mbcommon-shared
            ${MBP_JSONCPP_LIBRARIES}
        )
    endif()

    # Install shared library
    if(${variant} STREQUAL shared)
        install(
            TARGETS ${lib_target}
            LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
            RUNTIME DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
            #ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        )
    endif()
endforeach()

# Build tests
if(MBP_ENABLE_TESTS)
    foreach(variant ${variants})
        # Link against objects so we don't have to worry about hidden symbols
        set(obj_target mbdevice-${variant}-obj)

        # Build tests
        add_executable(
            mbdevice_tests
            ${MBDEVICE_TESTS_SOURCES}
            $<TARGET_OBJECTS:${obj_target}>
        )

        # Link dependencies
        target_link_libraries(
            mbdevice_tests
            ${GTEST_BOTH_LIBRARIES}
            ${MBP_JSONCPP_LIBRARIES}
        )

        # Target C++11
        if(NOT MSVC)
            set_target_properties(
                mbdevice_tests
                PROPERTIES
                CXX_STANDARD 11
                CXX_STANDARD_REQUIRED 1
            )
        endif()

        # Add to ctest
        add_test(
            NAME mbdevice_tests
            COMMAND mbdevice_tests
        )

        # Only need to build the tests once
        break()
    endforeach()
endif()
