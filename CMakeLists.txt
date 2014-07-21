cmake_minimum_required(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCC)
    if(ENABLE_OPENMP)
	   set(CMAKE_CXX_FLAGS "-fPIC -D_FILE_OFFSET_BITS=64 -O3 -fopenmp ${CMAKE_CXX_FLAGS}")
    else(ENABLE_OPENMP)
        set(CMAKE_CXX_FLAGS "-fPIC -D_FILE_OFFSET_BITS=64 -O3 ${CMAKE_CXX_FLAGS}")
    endif(ENABLE_OPENMP)
endif(CMAKE_COMPILER_IS_GNUCC)


if(APPLE)
    set( CMAKE_CXX_FLAGS " -m64 -lsqlite3 -flat_namespace -undefined suppress ${CMAKE_CXX_FLAGS}" )
    message(STATUS "Flags for Mac OS added " )
endif(APPLE)


# ----------------------------------------------------------------------------
# SQLite3
# ---------------------------------------------------------------------------- 

find_path(SQLITE3_INCLUDE_PATH sqlite3.h)
find_path(SQLITE3_LIBRARY_PATH libsqlite3)
find_library(SQLITE3_LIBRARY libsqlite3)
include_directories(${SQLITE3_INCLUDE_PATH})

if(SQLITE3_INCLUDE_PATH AND SQLITE3_LIBRARY)
    message(STATUS "SQLite3 found!")
    message(STATUS "  libs:        ${SQLITE3_LIBRARY}")
    message(STATUS "  include:     ${SQLITE3_INCLUDE_PATH}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread -lsqlite3 ${SQLITE3_LIBS} ")
else(SQLITE3_INCLUDE_PATH AND SQLITE3_LIBRARY)
    message(STATUS "SQLite3: NOT Found!")    
endif(SQLITE3_INCLUDE_PATH AND SQLITE3_LIBRARY)

# ----------------------------------------------------------------------------
# SAGA
# ----------------------------------------------------------------------------
include_directories (include)

add_library( saga-lib src/AMRgrid.cc src/AMRcell.cc src/LocalProperties.cc src/SQLiteInterface.cc src/MagneticField.cc )
set_target_properties(saga-lib PROPERTIES OUTPUT_NAME "saga")

install(TARGETS saga-lib DESTINATION lib)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")

set(SAGA_PREFIX ${CMAKE_INSTALL_PREFIX})
set(SAGA_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libSAGA.a)

set(SAGA_PREFIX ${CMAKE_SOURCE_DIR})
set(SAGA_LIBRARY ${CMAKE_BINARY_DIR}/libSAGA.a)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------------------------
# Testing (optional)
# ----------------------------------------------------------------------------
option(ENABLE_TESTING "Build unit tests" ON)
if(ENABLE_TESTING)
    enable_testing()
    add_executable( testMain test/mainTest.cc )
    target_link_libraries( testMain saga-lib )
    add_test(mainTest testMain)
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
