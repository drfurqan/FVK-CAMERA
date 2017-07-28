set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/examples)

set(INCLUDE_DIR
${PROJECT_SOURCE_DIR}/include
${OpenCV_INCLUDE_DIRS}
)

include_directories(${INCLUDE_DIR})

set(LIBRARIES 
${PROJECT_NAME} 
${OpenCV_LIBS}
)

add_executable (camera_program_1 camera_program_1.cpp)
target_link_libraries(camera_program_1 LINK_PUBLIC ${LIBRARIES})

add_executable (camera_program_2 camera_program_2.cpp)
target_link_libraries(camera_program_2 LINK_PUBLIC ${LIBRARIES})

add_executable (camera_program_3 camera_program_3.cpp)
target_link_libraries(camera_program_3 LINK_PUBLIC ${LIBRARIES})

add_executable (camera_program_4 camera_program_4.cpp)
target_link_libraries(camera_program_4 LINK_PUBLIC ${LIBRARIES})