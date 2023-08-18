
cmake_minimum_required(VERSION 3.16)

set(STM32_DEVICE "STM32F746xx")
set(MCU_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Src/STM32F746VGTx_FLASH.ld")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32_gcc.cmake)
#set(LFRAMEWORK_CONFIG_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

project(LedCircle CXX C ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32)

include(fetch)
FetchStm32F7Hal()
FetchStm32F7FreeRtos()
FetchStm32F7CmsisDevice()
FetchStm32F7CmsisCore()

find_package(Stm32F7Hal REQUIRED)
find_package(Stm32F7FreeRtos REQUIRED)
find_package(Stm32F7CmsisDevice REQUIRED)
find_package(Stm32F7CmsisCore REQUIRED)


add_executable(${PROJECT_NAME})

add_subdirectory(Src)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    Stm32F7Hal
    Stm32F7FreeRtos
    Stm32F7CmsisDevice
    Stm32F7CmsisCore
)

target_compile_definitions(${PROJECT_NAME} 
PRIVATE 
    ${STM32_DEVICE}
)

