# Find the jsoncpp include directory and library
#
# JSONCPP_INCLUDE_DIR - Where to find <json/json.h>
# JSONCPP_LIBRARIES   - List of jsoncpp libraries
# JSONCPP_FOUND       - True if jsoncpp found

# Find include directory
find_path(JSONCPP_INCLUDE_DIR NAMES json/json.h)

# Find library
find_library(JSONCPP_LIBRARY NAMES jsoncpp libjsoncpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    JSONCPP DEFAULT_MSG JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY
)

if(JSONCPP_FOUND)
    set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
endif()

mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)
