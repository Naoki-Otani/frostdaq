foreach(val IN ITEMS
    erase_eeprom;
    flash_memory_programmer;
    gen_user_reset;
    get_version;
    inject_sem_error;
    mcs_converter;
    read_register;
    read_scr;
    read_sem;
    read_xadc;
    reconfig_fpga;
    reset_sem;
    reset_sitcp;
    send_cbtinit;
    set_cdce62002;
    set_ctrl_eeprom;
    set_hbfstate;
    set_max1932;
    set_sitcpreg;
    show_laccp;
    show_mikumari;
    strdaq;
    verify_mcs;
    write_register
    )
  set(MY_TARGET ${val})
  add_executable(${MY_TARGET}
    ${MY_TARGET}_main.cc
    )

  if(${MYDEBUG})
    message(STATUS "MY_TARGET @ Common: " ${MY_TARGET})
  endif()

  target_compile_options(${MY_TARGET} PUBLIC
    ${my_cxx_options}
    )
  target_compile_features(${MY_TARGET} PUBLIC
    ${my_cxx_features}
    )
  target_include_directories(${MY_TARGET} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../HulCore
    ${CMAKE_CURRENT_SOURCE_DIR}/FeeCtrl
    )
  target_link_directories(${MY_TARGET} PUBLIC
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR};
    )
  target_link_libraries(${MY_TARGET} PUBLIC
    FeeCtrl;
    HulCore;
    )

  install(TARGETS ${MY_TARGET}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endforeach()

add_subdirectory(FeeCtrl)
