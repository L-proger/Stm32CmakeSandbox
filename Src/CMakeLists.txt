target_sources(${PROJECT_NAME} 
	PRIVATE 
		main.cpp 
		WS2812B.h
		Delay.h
		Color.h
		interrupts.c
		Stm32F7.h
)

#add_subdirectory(Cube)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces)


