
cmake_minimum_required(VERSION 3.16)

set(STM32_DEVICE "STM32F746xx")
set(MCU_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Src/Cube/STM32F746VGTx_FLASH.ld")



set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32_gcc.cmake)

#set(LFRAMEWORK_CONFIG_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

project(LedCircle CXX C ASM)


#stm32_fetch_cmsis()D

add_executable(${PROJECT_NAME} )

add_subdirectory(Src)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)




target_compile_definitions(${PROJECT_NAME} 
PRIVATE 
    ${STM32_DEVICE}
)



