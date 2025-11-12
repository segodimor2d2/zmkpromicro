# Recomendação realista
- Se você precisa de código .c adicional como uart_receiver.c, siga esta estrutura:
- Tenha um fork do repositório ZMK
- No seu fork do ZMK, edite CMakeLists.txt para incluir:
- add_subdirectory(${ZMK_CONFIG}/src)
- Mantenha tudo seu (configs e código) no zmk-config/, e só altere o CMakeLists.txt do ZMK uma vez.

## Vá até o final do arquivo zmk/app/CMakeLists.txt e adicione isso 
### Incluir código do zmk-config/src de fora do repositório
add_subdirectory(${ZMK_CONFIG}/src ${CMAKE_CURRENT_BINARY_DIR}/zmk_config_src)


## add button 4ta row on corne zmk/app/boards/shields/corne/corne.dtsi
### substituir RC(3
                        RC(3,3) RC(3,4) RC(3,5)  RC(3,6) RC(3,7) RC(3,8)
### por
RC(3,0) RC(3,1) RC(3,2) RC(3,3) RC(3,4) RC(3,5)  RC(3,6) RC(3,7) RC(3,8) RC(3,9) RC(3,10) RC(3,11)


## compilation test
// #error "!!!!VERIFICANDO SE ESTÁ SENDO COMPILADO!!!!"
#error "!!!!VERIFICANDO SE ESTÁ SENDO COMPILADO!!!!"

