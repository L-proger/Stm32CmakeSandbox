target_sources(${PROJECT_NAME} 
	PRIVATE 
		main.cpp 
		rtos.c
)

add_subdirectory(Cube)


target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces)


