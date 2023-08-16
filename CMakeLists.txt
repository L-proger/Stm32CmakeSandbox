
cmake_minimum_required(VERSION 3.16)

set(STM32_DEVICE "STM32F746xx")
set(MCU_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/Src/STM32F746VGTx_FLASH.ld")



set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/LFramework.CMake/stm32_gcc.cmake)

#set(LFRAMEWORK_CONFIG_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

project(LedCircle CXX C ASM)


#stm32_fetch_cmsis()D

add_executable(${PROJECT_NAME} )

add_subdirectory(Src)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)


FetchContent_Declare(stm32f7xx_hal_driver                              # name of the content
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32f7xx_hal_driver.git  # the repository
  GIT_TAG        v1.3.0
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(stm32f7xx_hal_driver)
if(NOT stm32f7xx_hal_driver_POPULATED)
  FetchContent_Populate(stm32f7xx_hal_driver)

  add_library(stm32f7xx_hal_driver INTERFACE)

  target_include_directories(stm32f7xx_hal_driver
  INTERFACE
      ${stm32f7xx_hal_driver_SOURCE_DIR}/Inc
      ${stm32f7xx_hal_driver_SOURCE_DIR}/Inc/Legacy

      
  )

  file(GLOB_RECURSE stm32_mw_freertos_HEADERS1 "${stm32f7xx_hal_driver_SOURCE_DIR}/Src/**/*.h")
  file(GLOB_RECURSE stm32_mw_freertos_SOURCES1 "${stm32f7xx_hal_driver_SOURCE_DIR}/Src/**/*.c")

  target_sources(stm32f7xx_hal_driver 
  INTERFACE 
    ${stm32_mw_freertos_HEADERS1}
    ${stm32_mw_freertos_SOURCES1}
  )
endif()





#build\_deps\cmsis_device_f7-src\Source\Templates\system_stm32f7xx.c



FetchContent_Declare(cmsis_device_f7                              # name of the content
  GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_device_f7.git  # the repository
  GIT_TAG        v1.2.8
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(cmsis_device_f7)
if(NOT cmsis_device_f7_POPULATED)
  FetchContent_Populate(cmsis_device_f7)

  add_library(cmsis_device_f7 INTERFACE)

  target_include_directories(cmsis_device_f7
  INTERFACE
      ${cmsis_device_f7_SOURCE_DIR}/Include

  )

  file(GLOB_RECURSE cmsis_device_f7_HEADERS1 "${cmsis_device_f7_SOURCE_DIR}/Include/**/*.h")
  #file(GLOB_RECURSE cmsis_device_f7_SOURCES1 "${cmsis_device_f7_SOURCE_DIR}/Src/**/*.c")

  target_sources(cmsis_device_f7 
  INTERFACE 

    ${cmsis_device_f7_SOURCE_DIR}/Source/Templates/system_stm32f7xx.c
    ${cmsis_device_f7_SOURCE_DIR}/Source/Templates/gcc/startup_stm32f746xx.s
    ${stm32_mw_freertos_HEADERS1}
  )
endif()








FetchContent_Declare(cmsis_core                              # name of the content
  GIT_REPOSITORY https://github.com/STMicroelectronics/cmsis_core.git  # the repository
  GIT_TAG        v5.6.0_cm7
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(cmsis_core)
if(NOT cmsis_core_POPULATED)
  FetchContent_Populate(cmsis_core)

  add_library(cmsis_core INTERFACE)

  target_include_directories(cmsis_core
  INTERFACE
    ${cmsis_core_SOURCE_DIR}/Core/Include

  )
message("cmsis_core_Include: ${cmsis_core_SOURCE_DIR}/Core/Include")
  file(GLOB_RECURSE cmsis_core_HEADERS1 "${cmsis_core_SOURCE_DIR}/Core/Include/**/*.h")
  #file(GLOB_RECURSE cmsis_device_f7_SOURCES1 "${cmsis_device_f7_SOURCE_DIR}/Src/**/*.c")

  target_sources(cmsis_core 
  INTERFACE 
    ${cmsis_core_HEADERS1}
  )
endif()










FetchContent_Declare(stm32_mw_freertos                              # name of the content
  GIT_REPOSITORY https://github.com/STMicroelectronics/stm32_mw_freertos.git  # the repository
  GIT_TAG        v10.4.6
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
)

FetchContent_GetProperties(stm32_mw_freertos)
if(NOT stm32_mw_freertos)
    FetchContent_Populate(stm32_mw_freertos)
    message("Rtos Source Dir: ${stm32_mw_freertos_SOURCE_DIR}" )

    add_library(stm32_mw_freertos INTERFACE)

    target_include_directories(stm32_mw_freertos
    INTERFACE
        ${stm32_mw_freertos_SOURCE_DIR}/Source
        ${stm32_mw_freertos_SOURCE_DIR}/Source/include
        ${stm32_mw_freertos_SOURCE_DIR}/Source/CMSIS_RTOS_V2
        ${stm32_mw_freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM7/r0p1
    )

    #file(GLOB_RECURSE stm32_mw_freertos_HEADERS1 "${stm32_mw_freertos_SOURCE_DIR}/Source/CMSIS_RTOS_V2/**/*.h")
    #file(GLOB_RECURSE stm32_mw_freertos_SOURCES1 "${stm32_mw_freertos_SOURCE_DIR}/Source/CMSIS_RTOS_V2/**/*.c")
    file(GLOB_RECURSE stm32_mw_freertos_HEADERS2 "${stm32_mw_freertos_SOURCE_DIR}/Source/include/**/*.h")
    file(GLOB stm32_mw_freertos_SOURCES2 "${stm32_mw_freertos_SOURCE_DIR}/Source/*.c")

    #file(GLOB_RECURSE stm32_mw_freertos_SOURCES3 "${stm32_mw_freertos_SOURCE_DIR}/Source/portable/Common/*.c")
    #file(GLOB_RECURSE stm32_mw_freertos_HEADERS3 "${stm32_mw_freertos_SOURCE_DIR}/Source/portable/Common/*.h")

    file(GLOB_RECURSE stm32_mw_freertos_SOURCES4 "${stm32_mw_freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM7/*.c")
    file(GLOB_RECURSE stm32_mw_freertos_HEADERS4 "${stm32_mw_freertos_SOURCE_DIR}/Source/portable/GCC/ARM_CM7/*.h")

  

    target_sources(stm32_mw_freertos 
    INTERFACE 
      ${stm32_mw_freertos_SOURCE_DIR}/Source/portable/MemMang/heap_4.c
        ${stm32_mw_freertos_HEADERS2} 
        #${stm32_mw_freertos_HEADERS3} 
        ${stm32_mw_freertos_HEADERS4} 

  
        ${stm32_mw_freertos_SOURCES2}
       # ${stm32_mw_freertos_SOURCES3}
        ${stm32_mw_freertos_SOURCES4}
    )
endif()











target_link_libraries(${PROJECT_NAME} 
PRIVATE 
    stm32f7xx_hal_driver
    stm32_mw_freertos
    cmsis_device_f7
    cmsis_core
)



target_compile_definitions(${PROJECT_NAME} 
PRIVATE 
    ${STM32_DEVICE}
)



