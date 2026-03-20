cmake_minimum_required(VERSION 3.8)
project(bot_control)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSERIAL REQUIRED libserial)

include_directories(include)
include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${LIBSERIAL_INCLUDE_DIRS})

add_executable(velocity_controller src/velocity_controller.cpp)
ament_target_dependencies(velocity_controller rclcpp std_msgs geometry_msgs ${Eigen_LIBRARIES})

add_executable(arduino_joystick_controller src/arduino_joystick_controller.cpp)
ament_target_dependencies(arduino_joystick_controller rclcpp std_msgs geometry_msgs)

target_link_libraries(arduino_joystick_controller ${LIBSERIAL_LIBRARIES})

install(TARGETS
  arduino_joystick_controller
  DESTINATION lib/${PROJECT_NAME}
)
install(
  DIRECTORY include
  DESTINATION include
)

install(TARGETS
  velocity_controller
  DESTINATION lib/${PROJECT_NAME}
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
