add_executable(jdb jdb.cpp)
target_link_libraries(jdb PRIVATE jdb::libjdb PkgConfig::libedit)

include(GNUInstallDirs)
install(
  TARGETS jdb
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

