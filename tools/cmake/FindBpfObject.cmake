if(NOT BPFOBJECT_CLANG_EXE)
  find_program(BPFOBJECT_CLANG_EXE NAMES clang DOC "Path to clang executable")

  execute_process(COMMAND ${BPFOBJECT_CLANG_EXE} --version
    OUTPUT_VARIABLE CLANG_version_output
    ERROR_VARIABLE CLANG_version_error
    RESULT_VARIABLE CLANG_version_result
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Check that clang is new enough
  if(${CLANG_version_result} EQUAL 0)
    if("${CLANG_version_output}" MATCHES "clang version ([^\n]+)\n")
      # Transform X.Y.Z into X;Y;Z which can then be interpreted as a list
      set(CLANG_VERSION "${CMAKE_MATCH_1}")
      string(REPLACE "." ";" CLANG_VERSION_LIST ${CLANG_VERSION})
      list(GET CLANG_VERSION_LIST 0 CLANG_VERSION_MAJOR)

      # Anything older than clang 10 doesn't really work
      string(COMPARE LESS ${CLANG_VERSION_MAJOR} 10 CLANG_VERSION_MAJOR_LT10)
      if(${CLANG_VERSION_MAJOR_LT10})
        message(FATAL_ERROR "clang ${CLANG_VERSION} is too old for BPF CO-RE")
      endif()

      message(STATUS "Found clang version: ${CLANG_VERSION}")
    else()
      message(FATAL_ERROR "Failed to parse clang version string: ${CLANG_version_output}")
    endif()
  else()
    message(FATAL_ERROR "Command \"${BPFOBJECT_CLANG_EXE} --version\" failed with output:\n${CLANG_version_error}")
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BpfObject
  REQUIRED_VARS
    BPFOBJECT_BPFTOOL_EXE
    BPFOBJECT_CLANG_EXE
    LIBBPF_INCLUDE_DIRS
    LIBBPF_LIBRARIES
    BPFOBJECT_VMLINUX_DIR)

# Get clang bpf system includes
execute_process(
  COMMAND bash -c "${BPFOBJECT_CLANG_EXE} -v -E - < /dev/null 2>&1 |
          sed -n '/<...> search starts here:/,/End of search list./{ s| \\(/.*\\)|-idirafter \\1|p }'"
  OUTPUT_VARIABLE CLANG_SYSTEM_INCLUDES_output
  ERROR_VARIABLE CLANG_SYSTEM_INCLUDES_error
  RESULT_VARIABLE CLANG_SYSTEM_INCLUDES_result
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(${CLANG_SYSTEM_INCLUDES_result} EQUAL 0)
  separate_arguments(CLANG_SYSTEM_INCLUDES UNIX_COMMAND ${CLANG_SYSTEM_INCLUDES_output})
  message(STATUS "BPF system include flags: ${CLANG_SYSTEM_INCLUDES}")
else()
  message(FATAL_ERROR "Failed to determine BPF system includes: ${CLANG_SYSTEM_INCLUDES_error}")
endif()

# Public macro
macro(bpf_object name input)
  set(BPF_C_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/${input})
  set(BPF_O_FILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.bpf.o)
  set(BPF_SKEL_FILE ${CMAKE_CURRENT_BINARY_DIR}/${name}.skel.h)
  set(OUTPUT_TARGET ${name}_skel)

  # Build BPF object file
  add_custom_command(OUTPUT ${BPF_O_FILE}
    COMMAND ${BPFOBJECT_CLANG_EXE} -g -O2 -target bpf -D__TARGET_ARCH_${ARCH}
            ${CLANG_SYSTEM_INCLUDES} -I${INCLUDE_DIR}
            -isystem ${LIBBPF_INCLUDE_DIRS} -c ${BPF_C_FILE} -o ${BPF_O_FILE}
    COMMAND_EXPAND_LISTS  # Without this, lists might be passed as single
                          # arguments, causing errors.
    VERBATIM  # Ensures the command is passed exactly as specified to the
              # shell, preserving all formatting, arguments, and special
              # characters. This avoids unintended changes during command
              # parsing.
    DEPENDS ${BPF_C_FILE}
    COMMENT "[peeker] Building BPF object: ${BPF_O_FILE}")

  # Build BPF skeleton header
  add_custom_command(OUTPUT ${BPF_SKEL_FILE}
    COMMAND bash -c "${BPFOBJECT_BPFTOOL_EXE} gen skeleton ${BPF_O_FILE} > ${BPF_SKEL_FILE}"
    VERBATIM
    DEPENDS ${BPF_O_FILE}
    COMMENT "[peeker]  Building BPF skeleton: ${BPF_SKEL_FILE}")

  add_library(${OUTPUT_TARGET} INTERFACE)
  target_sources(${OUTPUT_TARGET} INTERFACE ${BPF_SKEL_FILE})
  # INTERFACE: This means this directory will only affect targets that use
  # (link to) this library.
  target_include_directories(${OUTPUT_TARGET} INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
  # SYSTEM: Marks these directories as system headers to suppress warnings
  # about them during compilation.
  # INTERFACE: These include directories are propagated to other targets that
  # use this library.
  target_include_directories(${OUTPUT_TARGET} SYSTEM INTERFACE ${LIBBPF_INCLUDE_DIRS})
  # Specifies libraries that targets linking to ${OUTPUT_TARGET} will also link to
  target_link_libraries(${OUTPUT_TARGET} INTERFACE ${LIBBPF_LIBRARIES} -lelf -lz)
endmacro()