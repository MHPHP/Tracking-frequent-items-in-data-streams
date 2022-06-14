cmake_minimum_required(VERSION 3.21)
project(bachelor)

set(CMAKE_CXX_STANDARD 17)

# Add executable for each experiment:
add_executable(run_main main.cpp countMin.cpp countMin.h countSketch.h countSketch.cpp HeavyHitter.cpp HeavyHitter.h)
add_executable(cMinExperiment cMinExperiment.cpp countMin.h countMin.cpp)
add_executable(countMedianExperiment cSketchExperiment.cpp countSketch.h countSketch.cpp)
add_executable(heavyHitterExperiment1 HeavyHitterExperiment1.cpp HeavyHitter.cpp HeavyHitter.h countMin.h countMin.cpp)
add_executable(countMinLearnExperiment CountMinLearnExperiment.cpp countMinLearn.cpp countMinLearn.h)

add_executable(countSketchLearnExperiment CountSketchLearnExperiment.cpp countSketchLearn.cpp countSketchLearn.h)