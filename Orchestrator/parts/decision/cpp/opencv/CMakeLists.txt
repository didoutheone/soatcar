# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Soatcar Tools directory
set(TOOLS_DIR "../../../../tools/cpp")

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define project name
project(opencv_soatcar_project)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Define source files
set(SOURCES soatcar.cpp ${TOOLS_DIR}/SoatcarState.cpp)

# Declare the executable target built from your sources
add_executable(opencv_soatcar ${SOURCES})

# Add include dir
target_include_directories(opencv_soatcar PRIVATE ${TOOLS_DIR})

# Link your application with OpenCV libraries
target_link_libraries(opencv_soatcar ${OpenCV_LIBS})
