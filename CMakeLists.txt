cmake_minimum_required(VERSION 3.16)

project(circuit-sim)

# Windows specific config
IF (WIN32)
    message("Windows Config")
    # Include local sfml cmake config
    set(SFML_DIR "D:/circuit-sim/SFML-2.5.1/lib/cmake/SFML")
    # Link sfml statically (Optional)
    set(SFML_STATIC_LIBRARIES TRUE)

    find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)

    FILE(
      GLOB
      SOURCES
      "main.cpp"
    )

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)

    target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${PROJECT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
ENDIF(WIN32)

IF (UNIX)
  message("Unix Config")
project(circuit-sim)

file (GLOB SOURCES
  "main.cpp"
  "src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} 
  sfml-graphics 
  sfml-window 
  sfml-system
  tgui
)
ENDIF(UNIX)

