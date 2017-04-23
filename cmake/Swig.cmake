if(${MBP_BUILD_TARGET} STREQUAL android-app)
    # Enable warnings
    list(APPEND CMAKE_SWIG_FLAGS -Wall -Wextra)

    # Java package
    set(MBP_SWIG_JAVA_PACKAGE io.noobdev.dualbootpatcher.nativelib)
    string(REPLACE "." "/" MBP_SWIG_JAVA_PACKAGE_DIR ${MBP_SWIG_JAVA_PACKAGE})
    list(APPEND CMAKE_SWIG_FLAGS -package "${MBP_SWIG_JAVA_PACKAGE}")

    # Output directory
    set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}/swig_java_output")

    # Install
    install(
        DIRECTORY "${CMAKE_SWIG_OUTDIR}/"
        DESTINATION "${JAVA_INSTALL_DIR}/${MBP_SWIG_JAVA_PACKAGE_DIR}"
        COMPONENT Libraries
        FILES_MATCHING PATTERN "*.java"
    )

    # Clean target
    add_custom_target(
        clean-swig
        ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SWIG_OUTDIR}"
        VERBATIM
    )
endif()
