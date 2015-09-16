cmake_minimum_required(VERSION 2.6)
project(SAGA C CXX)

set(CMAKE_MACOSX_RPATH 1)


set(CMAKE_C_FLAGS " -O3  -ldl -lpthread -fPIC -D_FILE_OFFSET_BITS=64 ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS " -O3 -ldl -lpthread -fPIC -D_FILE_OFFSET_BITS=64 ${CMAKE_CXX_FLAGS}")
if(CMAKE_COMPILER_IS_GNUCC)
       set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS}")
endif(CMAKE_COMPILER_IS_GNUCC)


if(APPLE)
    set(CMAKE_CXX_FLAGS "  -flat_namespace -undefined suppress ${CMAKE_CXX_FLAGS}" )
    message(STATUS "Flags for Mac OS added ")
endif(APPLE)


# ----------------------------------------------------------------------------
# OpenMP
# ---------------------------------------------------------------------------- 

# OpenMP (optional for shared memory multiprocessing)
option(ENABLE_OPENMP "OpenMP for multithreading" ON)
if(ENABLE_OPENMP)
include(FindOpenMP)
    if(OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif(OPENMP_FOUND)
endif(ENABLE_OPENMP)


# ----------------------------------------------------------------------------
# SQLite3
# ----------------------------------------------------------------------------

include_directories(include/sqlite3)
add_library(sagaSQLite3 SHARED src/sqlite3.c include/sqlite3/sqlite3.h include/sqlite3/sqlite3ext.h)
add_executable(sagaSQLite src/sqlite3.c src/shell.c include/sqlite3/sqlite3.h include/sqlite3/sqlite3ext.h)
set_target_properties(sagaSQLite PROPERTIES OUTPUT_NAME sagaSQLite3)

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_THREADSAFE=2)

install(FILES include/sqlite3/sqlite3.h include/sqlite3/sqlite3ext.h DESTINATION include)
install(TARGETS sagaSQLite3 LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(TARGETS sagaSQLite RUNTIME DESTINATION bin)


# ----------------------------------------------------------------------------
# SAGA
# ----------------------------------------------------------------------------
include_directories(include)
add_library(saga-lib SHARED src/AMRgrid.cc src/AMRcell.cc src/LocalProperties.cc src/SQLiteInterface.cc src/MagneticField.cc src/sqlite3.c)
set_target_properties(saga-lib PROPERTIES OUTPUT_NAME "saga")

install(TARGETS saga-lib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")

set(CMAKE_CXX_FLAGS " -O3 -L${CMAKE_INSTALL_PREFIX} ${CMAKE_CXX_FLAGS}")

set(SAGA_PREFIX ${CMAKE_INSTALL_PREFIX})
set(SAGA_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libsaga.a)

set(SAGA_PREFIX ${CMAKE_SOURCE_DIR})
set(SAGA_LIBRARY ${CMAKE_BINARY_DIR}/libSAGA.a)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# ----------------------------------------------------------------------------
# Useful tools
# ----------------------------------------------------------------------------

add_executable(SampleUniformGrid utilities/SampleUniformGrid.cpp)
target_link_libraries(SampleUniformGrid saga-lib)

add_executable(GetListOfSources utilities/GetListOfSources.cpp)
target_link_libraries(GetListOfSources saga-lib)

add_executable(GetListOfSources2 utilities/GetListOfSources2.cpp)
target_link_libraries(GetListOfSources2 saga-lib)

# ----------------------------------------------------------------------------
# Testing (optional)
# ----------------------------------------------------------------------------
option(ENABLE_TESTING "Build unit tests" ON)
if(ENABLE_TESTING)
    enable_testing()
    add_executable(testMain test/mainTest.cc)
    target_link_libraries(testMain saga-lib)
    add_test(testMain testMain)
endif(ENABLE_TESTING)


# ----------------------------------------------------------------------------
# Python (optional)
# ----------------------------------------------------------------------------
option(ENABLE_PYTHON "Create python library via SWIG" ON)
if(ENABLE_PYTHON)
    include(python/Python.cmake)
    include_directories(${PYTHON_INCLUDE_PATH})
    
    file(GLOB_RECURSE SAGA_INCLUDES include/*.h)
    set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/saga_wrap.cxx PROPERTIES GENERATED true )
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/saga_wrap.cxx
                    COMMAND swig -c++ -python ${SAGA_SWIG_DEFINES} -I${CMAKE_SOURCE_DIR}/include -o  ${CMAKE_CURRENT_BINARY_DIR}/saga_wrap.cxx -outdir ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/python/saga.i 
                    DEPENDS ${CMAKE_SOURCE_DIR}/python/saga.i DEPENDS ${SAGA_INCLUDES} )
    add_library( saga-swig MODULE ${CMAKE_CURRENT_BINARY_DIR}/saga_wrap.cxx)
    set_target_properties(saga-swig PROPERTIES PREFIX "")
    set_target_properties(saga-swig PROPERTIES OUTPUT_NAME "_saga") 
    target_link_libraries(saga-swig saga-lib ${PYTHON_LIBRARIES})
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/saga.py" DESTINATION ${PYTHON_SITE_PACKAGES})
    install(TARGETS saga-swig LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES})
    install(FILES python/saga.i DESTINATION share/saga)
endif(ENABLE_PYTHON)



# ----------------------------------------------------------------------------
# Print feature overview
# ----------------------------------------------------------------------------
message(STATUS "Features:")

if(ENABLE_OPENMP)
    message(STATUS "  OpenMP:      Yes")
else(ENABLE_OPENMP)
    message(STATUS "  OpenMP:      No")
endif(ENABLE_OPENMP)

if(ENABLE_PYTHON)
    message(STATUS "  Python:      Yes")
else(ENABLE_PYTHON)
    message(STATUS "  Python:      No")
endif(ENABLE_PYTHON)

if(ENABLE_TESTING)
    message(STATUS "  Testing:     Yes")
else(ENABLE_TESTING)
    message(STATUS "  Testing:     No")
endif(ENABLE_TESTING)