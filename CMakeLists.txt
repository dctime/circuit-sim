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

file (GLOB SOURCES
  "src/UIElements/*.h"
  "src/UIElements/*.cpp"
  "src/Elements/*.h"
  "src/Elements/*.cpp"
  "main.cpp"
  "src/*.h"
  "src/*.cpp"
)

# find_package(OpenMP)
# if (OPENMP_FOUND)
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

# find_package(BLAS)


add_executable(${PROJECT_NAME} ${SOURCES})

# if (BLAS_FOUND)
#   message(STATUS "BLAS FOUND")
#   target_compile_definitions(${PROJECT_NAME}
#     PRIVATE
#       EIGEN_USE_BLAS
#   )
#   message(${BLAS_LIBRARIES})
#   target_link_libraries(${PROJECT_NAME}
#     PUBLIC
#     ${BLAS_LIBRARIES}
#   )
# else()
#   message(STATUS "BLAS NOT FOUND")
# endif()

target_include_directories(${PROJECT_NAME} PUBLIC src)
target_include_directories(${PROJECT_NAME} PUBLIC src/Elements)
target_include_directories(${PROJECT_NAME} PUBLIC src/UIElements)

target_link_libraries(${PROJECT_NAME} 
  PRIVATE
  sfml-graphics 
  sfml-window 
  sfml-system
  # tgui
  # OpenMP::OpenMP_CXX
)

# add_compile_options(${PROJECT_NAME} -fopenmp=libomp)

      set(CMAKE_THREAD_LIBS_INIT "-lpthread")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
      set(CMAKE_HAVE_THREADS_LIBRARY 1)
      set(CMAKE_USE_WIN32_THREADS_INIT 0)
      set(CMAKE_USE_PTHREADS_INIT 1)
      set(THREADS_PREFER_PTHREAD_FLAG ON)

ENDIF(UNIX)

