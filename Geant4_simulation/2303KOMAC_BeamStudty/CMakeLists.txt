#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(B1)

#----------------------------------------------------------------------------
# link Geant4 and ROOT library
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
set(ACTIVE_EVE OFF CACHE INTERNAL "")
if(ACTIVE_EVE)
	add_definitions(-DACTIVE_EVE)
endif(ACTIVE_EVE)
if(ACTIVE_EVE)
	find_package(ROOT REQUIRED COMPONENTS RIO Net Eve EG RGL Geom Gui Graf Ged Matrix MathCore)
else(ACTIVE_EVE)
	find_package(ROOT REQUIRED COMPONENTS RIO Net Geom Gui Graf Ged Matrix MathCore)
endif(ACTIVE_EVE)

include(${ROOT_USE_FILE})
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
add_executable(G4sim G4sim.cc ${sources} ${headers})
target_link_libraries(G4sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory.
set(EXAMPLEB1_SCRIPTS
  Parameters.conf
  init_vis.mac
  vis.mac
  run1.mac
  )

foreach(_script ${EXAMPLEB1_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

