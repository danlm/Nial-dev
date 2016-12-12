
# A generic cmake for building Q'Nial versions for Linux and Mac OSX. 
# Both 32 and 64 bit versions on Intel processors.
#

cmake_minimum_required (VERSION 2.6)
project(nopengl)

#--------------------  Build Options -------------------------

option(USE_INTS32 "Build a 32 bit system" OFF)
option(USE_INTS64 "Build a 64 bit system" ON)
option(USE_FASTMATH "Utilise compiler options to speed up maths" ON)  

set (OPTFLAGS "-O2")
if (USE_FASTMATH)
  set (OPTFLAGS "${OPTFLAGS} -ffast-math")
endif (USE_FASTMATH)

include_directories( /home/johng/QNial7/src)


# ------------------- System Specific -----------------------

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTFLAGS}")

# Linux specific settings
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX -DUNIXSYS")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# OSX specific flags
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOSX -DOSX_INTEL -DUNIXSYS")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")




# -------------------- Module Sources ---------------------

# Source files for the OpenCL module
set (MODULE_SRCS nopengl.c nopengl_supp.c)


# ---------------- Standard Libraries --------------------

# Always present on a build
set (MODULE_LIBS m util)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set (MODULE_LIBS ${MODULE_LIBS} GL GLU glut)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set (MODULE_LIBS "${MODULE_LIBS} -framework OpenGL")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


# ---------------- Extension Options ---------------------

# Compilation options for 32 bit system
if (USE_INTS32)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DINTS32")
endif (USE_INTS32)

# Compilation options for 64 bit system
if (USE_INTS64)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DINTS64")
endif (USE_INTS64)

# ---------------- Core Executable -------------------------


add_library(nopengl SHARED ${MODULE_SRCS})
target_link_libraries(nopengl ${MODULE_LIBS})



