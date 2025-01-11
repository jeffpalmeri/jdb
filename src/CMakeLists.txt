add_library(libjdb libjdb.cpp)
add_library(jdb::libjdb ALIAS libjdb)

set_target_properties(
  libjdb
  PROPERTIES OUTPUT_NAME jdb
)

target_compile_features(libjdb PUBLIC cxx_std_17)

# target_include_directories(
#   libjdb
#   PRIVATE include
#   PUBLIC ${CMAKE_SOURCE_DIR}/include
# )

target_include_directories(libjdb
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
)

include(GNUInstallDirs)
install(TARGETS libjdb
  EXPORT jdb-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY ${PROJECT_SOURCE-DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT jdb-targets
  FILE jdb-config.cmake
  NAMESPACE jdb::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jdb
)
