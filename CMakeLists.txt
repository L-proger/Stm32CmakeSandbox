
cmake_minimum_required(VERSION 3.16)

set(STM32_DEVICE "STM32F746xx")
set(MCU_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Src/STM32F746VGTx_FLASH.ld")
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32_gcc.cmake)
#set(LFRAMEWORK_CONFIG_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

project(LedCircle CXX C ASM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32)

set(STM32_DEVICE STM32F746VGT6)


include(fetch)
FetchStm32Hal()
FetchStm32FreeRtos()
FetchStm32CmsisDevice()
FetchStm32CmsisCore()

find_package(Stm32Hal REQUIRED)
find_package(Stm32FreeRtos REQUIRED)
find_package(Stm32CmsisDevice REQUIRED)
find_package(Stm32CmsisCore REQUIRED)







add_executable(${PROJECT_NAME})

add_subdirectory(Src)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    Stm32Hal
    Stm32FreeRtos
    Stm32CmsisDevice
    Stm32CmsisCore
)

target_compile_definitions(${PROJECT_NAME} 
PRIVATE 
    ${STM32_DEVICE}
)

