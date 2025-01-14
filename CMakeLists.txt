cmake_minimum_required (VERSION 3.1)
project (LSLOpt VERSION 1.0)

# perform coverage report
option(LSLOpt_WITH_COVERAGE "Enable coverage report analysis" OFF)

# build tests and examples
option(LSLOpt_BUILD_TESTS "Enable testing" ON)

# set the include directories for boost and Eigen
set(BOOST_INCLUDE_DIR "" CACHE STRING "Include dir where to find boost")
set(EIGEN_INCLUDE_DIR "" CACHE STRING "Include dir where to find Eigen")

set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

add_subdirectory("${INCLUDE_DIR}")

if (LSLOpt_BUILD_TESTS)
  # enable the CMake testing
  enable_testing()

  # currently needed by google test on macOS
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  if (NOT (EIGEN_INCLUDE_DIR STREQUAL ""))
    message("Using Eigen3 from ${EIGEN_INCLUDE_DIR}")
    include_directories(BEFORE SYSTEM ${EIGEN_INCLUDE_DIR})
  endif()

  if (NOT (BOOST_INCLUDE_DIR STREQUAL ""))
    message("Using Boost from ${BOOST_INCLUDE_DIR}")
    include_directories(BEFORE SYSTEM ${BOOST_INCLUDE_DIR})
  endif()

  # Download and unpack googletest at configure time
  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Add make target for test compilation and execution
  # add -V if you like to see the full GTEST output
  add_custom_target(build_and_test ${CMAKE_CTEST_COMMAND})

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                   ${CMAKE_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)
  add_subdirectory("${SOURCE_DIR}/Test")
  add_subdirectory("${SOURCE_DIR}/Examples")
  add_subdirectory("${SOURCE_DIR}/PaperExamples")

  # generate meaningful coverage report. Requires the lcov tool
  if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${LSLOpt_WITH_COVERAGE})
    message(STATUS "Enabled coverage build")

    add_custom_target(extract_coverage COMMAND /usr/bin/lcov --quiet --capture --output-file allCoverage.info --directory src/Test/
                      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMENT "Extract all coverage information" DEPENDS "build_and_test"
                      USES_TERMINAL VERBATIM BYPRODUCTS allCoverage.info)
    add_custom_target(filter_coverage COMMAND lcov --quiet --extract allCoverage.info '*include/LSLOpt*' --output-file LSLOptcoverage.info
                      DEPENDS "extract_coverage" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                      COMMENT "Filter LSLOpt coverage" BYPRODUCTS LSLOptcoverage.info)
    add_custom_target(generate_report COMMAND genhtml --demangle-cpp LSLOptcoverage.info --output-directory ${CMAKE_SOURCE_DIR}/LSLOpt-coverage-report
                      DEPENDS "filter_coverage" WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMENT "Generate HTML report in SRC directory"
                      BYPRODUCTS ${CMAKE_SOURCE_DIR}/LSLOpt-coverage-report)
    add_custom_target(clear_counters COMMAND lcov --zerocounters --directory . DEPENDS "extract_coverage"
                      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}" COMMENT "Cleanup call counters")
    add_custom_target(coverage_report COMMAND echo Please run: \"sensible-browser ${CMAKE_SOURCE_DIR}/LSLOpt-coverage-report/index.html\" to inspect the coverage report
                      DEPENDS "generate_report" "clear_counters" COMMENT "Execute whole report chain")
  endif()
endif()
