# This file is part of the MobiusCore project.
# See AUTHORS file for copyright information.

CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

if (USE_COREPCH)
  set(PRIVATE_PCH_HEADER PrecompiledHeaders/protoPCH.h)
endif()

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DSERVER_API_EXPORT_PROTO)

add_library(proto
  ${PRIVATE_SOURCES}
)

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  # Exclude
  ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders)

target_include_directories(proto
  PUBLIC
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(proto
  PRIVATE
    mobius-default-interface
    mobius-no-warning-interface
  PUBLIC
    common
    protobuf)

if (MSVC)
  target_compile_definitions(protobuf
    PRIVATE
      -D_SCL_SECURE_NO_WARNINGS)
endif()

set_target_properties(proto
  PROPERTIES
    FOLDER
      "server")

if( BUILD_SHARED_LIBS )
  if( UNIX )
    install(TARGETS proto
      LIBRARY
        DESTINATION lib)
  elseif( WIN32 )
    install(TARGETS proto
      RUNTIME
        DESTINATION "${CMAKE_INSTALL_PREFIX}")
  endif()
endif()

# Generate precompiled header
if (USE_COREPCH)
  add_cxx_pch(proto ${PRIVATE_PCH_HEADER})
endif()
