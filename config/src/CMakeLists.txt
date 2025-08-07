target_sources(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/uart_receiver.c
  ${CMAKE_CURRENT_LIST_DIR}/uart_switch.c
  # ${CMAKE_CURRENT_LIST_DIR}/fake_switch_init.c
)

zephyr_include_directories(${ZMK_CONFIG}/include)
