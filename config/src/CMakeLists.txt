# Inclui diretórios de headers
zephyr_include_directories(${ZMK_CONFIG}/include)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

# # Fonte comum (sempre incluída)
target_sources(app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/mouse_split_event.c
  ${CMAKE_CURRENT_LIST_DIR}/led_debug.c
)

if(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
  # Central (lado esquerdo)
  target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/uart_receiver_left.c
    ${CMAKE_CURRENT_LIST_DIR}/uart_switch_left.c
    ${CMAKE_CURRENT_LIST_DIR}/uart_move_mouse_left.c
    ${CMAKE_CURRENT_LIST_DIR}/split_mouse_central.c
    # ${CMAKE_CURRENT_LIST_DIR}/mouse_listener.c
    # ${CMAKE_CURRENT_LIST_DIR}/test_mouse_l_led.c
    # ${CMAKE_CURRENT_LIST_DIR}/test_mouse_l.c
    # ${CMAKE_CURRENT_LIST_DIR}/mouse_state_listener.
  )
else()
  # Peripheral (lado direito)
  target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/uart_receiver_right.c
    ${CMAKE_CURRENT_LIST_DIR}/uart_switch_right.c
    ${CMAKE_CURRENT_LIST_DIR}/uart_move_mouse_right.c
    ${CMAKE_CURRENT_LIST_DIR}/split_mouse_service.c
    # ${CMAKE_CURRENT_LIST_DIR}/test_mouse_r.c
    # ${CMAKE_CURRENT_LIST_DIR}/split_mouse_tx.c
  )
endif()
