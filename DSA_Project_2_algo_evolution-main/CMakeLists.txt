cmake_minimum_required(VERSION 3.20)
project(algo_evolution CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O2 -DNDEBUG)

# Ensure executables are output directly in build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR})
endforeach()

# =============================
#  Experiment executable
# =============================
add_executable(experiment
  src/common.cpp
  src/datasets.cpp
  src/quicksort.cpp
  src/mergesort.cpp
  src/evaluator.cpp
  src/ga.cpp
  src/sa.cpp
  src/logging.cpp
  src/main_experiment.cpp
)
target_include_directories(experiment PRIVATE include)

# =============================
#  Demo Executables
# =============================
add_executable(massive_demo src/massive_demo.cpp)

# =============================
#  HTML Visualization Generator
# =============================
add_executable(massive_html_gen src/massive_html_gen.cpp)
target_include_directories(massive_html_gen PRIVATE include)

# =============================
#  Developer convenience flags
# =============================
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "Configured algo-evolution project successfully.")
