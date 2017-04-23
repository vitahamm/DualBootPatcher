set(SWIGCOMMON_SWIG_SOURCE swig/swigcommon/swigcommon.i)

# swig
if(${MBP_BUILD_TARGET} STREQUAL android-app)
    set_source_files_properties(
        ${SWIGCOMMON_SWIG_SOURCE}
        PROPERTIES
        CPLUSPLUS ON
    )

    swig_add_module(
        swigcommon-jni
        java
        ${SWIGCOMMON_SWIG_SOURCE}
    )

    if(NOT MSVC)
        set_target_properties(
            swigcommon-jni
            PROPERTIES
            CXX_STANDARD 11
            CXX_STANDARD_REQUIRED 1
        )
    endif()

    target_compile_options(
        swigcommon-jni
        PRIVATE
        -fexceptions
    )

    install(
        TARGETS swigcommon-jni
        LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        RUNTIME DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
        #ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Libraries
    )
endif()
