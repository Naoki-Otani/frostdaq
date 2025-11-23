set(MY_TARGET FeeCtrl)
set(MY_PUB_HEADER
  "CDCE62002RegMap.hh"
  "CDCE62002Funcs.hh"
  "FlashMemoryProgrammer.hh"
  "SPIFlashMemoryMap.hh"
  "SitcpController.hh"
  "RegisterMapCommon.hh"
  )

file(GLOB_RECURSE SRCS "*.cc")

add_library(${MY_TARGET} STATIC
  ${SRCS}
  )

target_compile_options(${MY_TARGET} PRIVATE
  ${my_cxx_options}
  )
target_compile_features(${MY_TARGET} PRIVATE
  ${my_cxx_features}
  )
target_include_directories(${MY_TARGET} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../HulCore/ 
  )

set_target_properties(${MY_TARGET} PROPERTIES
  PUBLIC_HEADER "${MY_PUB_HEADER}"
  )

install(TARGETS ${MY_TARGET}
  ARCHIVE       DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
