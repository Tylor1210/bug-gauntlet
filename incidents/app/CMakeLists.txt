# linkpage-sim: the Phase 2 "link page service" used for injected incidents.
find_package(Threads REQUIRED)
add_executable(linkpage-sim main.cpp store.cpp analytics.cpp simulator.cpp log.cpp)
target_link_libraries(linkpage-sim PRIVATE Threads::Threads)
