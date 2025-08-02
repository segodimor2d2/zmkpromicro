# Recomendação realista
Se você precisa de código .c adicional como uart_receiver.c, siga esta estrutura:

Tenha um fork do repositório ZMK

No seu fork do ZMK, edite CMakeLists.txt para incluir:

add_subdirectory(${ZMK_CONFIG}/src)

Mantenha tudo seu (configs e código) no zmk-config/, e só altere o CMakeLists.txt do ZMK uma vez.

## Vá até o final do arquivo e adicione isso:

# Incluir código do zmk-config/src de fora do repositório
add_subdirectory(${ZMK_CONFIG}/src ${CMAKE_CURRENT_BINARY_DIR}/zmk_config_src)
