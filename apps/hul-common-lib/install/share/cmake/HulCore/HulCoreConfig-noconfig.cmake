#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "HulCore" for configuration ""
set_property(TARGET HulCore APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(HulCore PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libHulCore.so"
  IMPORTED_SONAME_NOCONFIG "libHulCore.so"
  )

list(APPEND _cmake_import_check_targets HulCore )
list(APPEND _cmake_import_check_files_for_HulCore "${_IMPORT_PREFIX}/lib/libHulCore.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
