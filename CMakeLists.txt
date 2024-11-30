cmake_minimum_required(VERSION 3.16)
project(peeker)

# CMAKE_MODULE_PATH is a list dirs where CMake looks for modules.
# list is a command for manipulating list variable.
# APPEND adds new elements to the end of the existing list variable.
# CMAKE_CURRENT_SOURCE_DIR represents the full path to the directory
#   containing the CMakeLists.txt or .cmake file currently being processed.
list(APPEND CMAKE_MODULE_PATH $(CMAKE_CURRENT_SOURCE_DIR)/tools/cmake)

# ExternalPeoject is a CMake's standard module, it provides tools for
# building, downloading, and managing external projects as part of your
# CMake-based build system.
# include() loads and executes the specified module or script file.
include(ExternalProject)

set(INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(TOOL_DIR ${CMAKE_CURRENT_BINARY_DIR}/tools)
set(LIBRARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs)

set(BPFOBJECT_VMLINUX_DIR ${INCLUDE_DIR})
set(BPFOBJECT_BPFTOOL_EXE ${TOOL_DIR}/bpftool)
set(BPFOBJECT_VMLINUX_H ${BPFOBJECT_VMLINUX_DIR}/vmlinux.h)
set(LIBBPF_INCLUDE_DIRS ${INCLUDE_DIR})
set(LIBBPF_LIBRARIES ${LIBRARY_DIR})

# To create the directorys if there is not.
if(NOT EXISTS "${INCLUDE_DIR}")
  file(MAKE_DIRECTORY "${INCLUDE_DIR}")
  message(STATUS "[peeker] ${INCLUDE_DIR} didn't exist and has been created.")
endif()
if(NOT EXISTS "${LIBRARY_DIR}")
  file(MAKE_DIRECTORY "${LIBRARY_DIR}")
  message(STATUS "[peeker] ${LIBRARY_DIR} didn't exist and has been created.")
endif()
if(NOT EXISTS "${TOOL_DIR}")
  file(MAKE_DIRECTORY "${TOOL_DIR}")
  message(STATUS "[peeker] ${TOOL_DIR} didn't exist and has been created.")
endif()

# ExternalProject_Add function will create a external project
ExternalProject_Add(libbpf  # the project name which will be managed
                    # and managed as a library.
  PREFIX libbpf     # Specifies the base directory for all files associated
                    # with this external project, including download, build
                    # and installation. Its full path will be relative to the
                    # build directory (${CMAKE_BINARY_DIR}/libbpf).
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libbpf/src  # Specifies the location
                    # of the source code for the external project.
  CONFIGURE_COMMAND ""  # Defines the command to configure the external project
                    # before building.
  BUILD_COMMAND make    # means use make to build the external project
    BUILD_STATIC_ONLY=1 # Ensures that only the static version of the
                    # library is built.
    OBJDIR=${CMAKE_CURRENT_BINARY_DIR}/libbpf # Sets the directory for
                    # intermediate object files to a subdirectory in the build
                    # directory.
    DESTDIR=${INCLUDE_DIR}  # Specifies the destination
                    #  directory for the `make install` command.
    INCLUDEDIR=
    LIBDIR=
    UAPIDIR=
    install install_uapi_headers  # execute `make install install_uapi_headers`
  BUILD_IN_SOURCE TRUE
  INSTALL_COMMAND zsh -c "test -e ${INCLUDE_DIR}/libbpf.a && mv -f ${INCLUDE_DIR}/pkgconfig ${INCLUDE_DIR}/libbpf.a ${LIBRARY_DIR}"
  STEP_TARGETS build install
)

ExternalProject_Add(bpftool
  PREFIX bpftool
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bpftool/src
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make bootstrap  # This is a Makefile target, to ensure that
                    # all necessary components and dependencies are available
                    # and correctly compiled.
    OUTPUT=${CMAKE_CURRENT_BINARY_DIR}/bpftool/
  BUILD_IN_SOURCE TRUE
  INSTALL_COMMAND zsh -c "test -e ${CMAKE_CURRENT_BINARY_DIR}/bpftool/bootstrap/bpftool && mv ${CMAKE_CURRENT_BINARY_DIR}/bpftool/bootstrap/bpftool ${TOOL_DIR}"
  STEP_TARGETS build install
)

# It's not effective to use `zsh -c` in this function!
add_custom_target(generate_vmlinux ALL  # ALL: Makes this custom target part
                    # of the default build process.
  COMMAND ${TOOL_DIR}/bpftool btf dump file /sys/kernel/btf/vmlinux format c > ${BPFOBJECT_VMLINUX_H}
  DEPENDS bpftool-build # The "-build" suffix refers to the build step of
                    # `ExternalProject_Add`
)

find_program(CARGO_EXISTS cargo)
if(CARGO_EXISTS)
  ExternalProject_Add(blazesym
    PREFIX blazesym
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blazesym
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build --release
    BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""
    STEP_TARGETS build
  )
endif()

# [:============  BpfObject Module Configurations ============:]
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
  set(ARCH "x86")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
  set(ARCH "arm")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
  set(ARCH "arm64")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc64le")
  set(ARCH "powerpc")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "mips")
  set(ARCH "mips")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "riscv64")
  set(ARCH "riscv")
elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "loongarch64")
  set(ARCH "loongarch")
endif()

# find_package(BpfObject REQUIRED)