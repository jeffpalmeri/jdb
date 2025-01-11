add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE jdb::libjdb Catch2::Catch2WithMain)
