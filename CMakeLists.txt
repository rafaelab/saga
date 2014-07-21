cmake_minimum_required(VERSION 2.6)

if(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_CXX_FLAGS "-fPIC -D_FILE_OFFSET_BITS=64 -O3 -fopenmp ${CMAKE_CXX_FLAGS}")
endif()

#  -Wall -O3 -m64
# 

IF(APPLE)
    set( CMAKE_CXX_FLAGS " -m64 -lsqlite3 -flat_namespace -undefined suppress ${CMAKE_CXX_FLAGS}" )
    message(STATUS "Flags for Mac OS added " )
ENDIF(APPLE)



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
else()
    message(STATUS "SQLite3: NOT Found!")    
endif()

# ----------------------------------------------------------------------------
# SAGA
# ----------------------------------------------------------------------------
include_directories (include)

add_library( saga-lib src/AMRgrid.cc src/AMRcell.cc src/LocalProperties.cc src/SQLiteInterface.cc src/MagneticField.cc )
SET_TARGET_PROPERTIES(saga-lib PROPERTIES OUTPUT_NAME "saga")

add_executable( saga-tool tools/profiles.cc)
SET_TARGET_PROPERTIES(saga-tool PROPERTIES OUTPUT_NAME "profiles")
add_dependencies(saga-tool saga-lib) 
target_link_libraries (saga-tool saga-lib) 

INSTALL(TARGETS saga-tool RUNTIME DESTINATION bin)
INSTALL(TARGETS saga-lib DESTINATION lib)
INSTALL(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
#if(UNIX)
    SET(SAGA_PREFIX ${CMAKE_INSTALL_PREFIX})
    SET(SAGA_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/libSAGA.a)

    SET(SAGA_PREFIX ${CMAKE_SOURCE_DIR})
    SET(SAGA_LIBRARY ${CMAKE_BINARY_DIR}/libSAGA.a)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#endif(UNIX)

# ----------------------------------------------------------------------------
# Testing (optional)
# ----------------------------------------------------------------------------
option(ENABLE_TESTING "Build unit tests" ON)
if(ENABLE_TESTING)
    enable_testing()
    add_executable( threads test/threads.cc )
    target_link_libraries( threads saga-lib )

    add_executable( testMain test/main.cc )
    target_link_libraries( testMain saga-lib )

    ADD_TEST(threadsTest threads)
    ADD_TEST(mainTest getField)
endif()

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

endif()



# ----------------------------------------------------------------------------
# Print feature overview
# ----------------------------------------------------------------------------
message(STATUS "Features:")


if(ENABLE_PYTHON)
	message(STATUS "  Python:      Yes")
else()
	message(STATUS "  Python:      No")
endif()

if(ENABLE_TESTING)
	message(STATUS "  Testing:     Yes")
else()
	message(STATUS "  Testing:     No")
endif()
