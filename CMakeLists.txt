project(battery_sim CXX)

find_package(Boost REQUIRED QUIET COMPONENTS program_options system)
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS i2c transport)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

file(GLOB bat_sim_h inc/*.hpp)

set(bat_sim_src src/config.cpp
                src/battery.cpp
                src/battery_sim.cpp)

set(bat_sim_libs pthread
                 ${Boost_LIBRARIES}
                 ${ITC_Common_itc_logger_LIBRARY}
                 ${NOSENGINE_LIBRARIES})

add_executable(nos3-battery-simulator ${bat_sim_h} ${bat_sim_src})
target_link_libraries(nos3-battery-simulator ${bat_sim_libs})

install(TARGETS nos3-battery-simulator RUNTIME DESTINATION bin)
install(FILES cfg/batteries.json DESTINATION bin)

