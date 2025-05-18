# fmt
find_package(fmt REQUIRED)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${fmt_LIBRARIES})

# yaml-cpp
find_package(yaml-cpp REQUIRED)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${yaml-cpp_LIBRARIES})

# spdlog
find_package(spdlog REQUIRED)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${spdlog_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE
    -DSPDLOG_HEADER_ONLY
    -DSPDLOG_DISABLE_DEFAULT_LOGGER
    -DSPDLOG_FMT_EXTERNAL
)

# boost.asio
find_package(asio REQUIRED)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${asio_LIBRARIES})

# boost
find_package(Boost REQUIRED)
list(APPEND CMAKE_REQUIRED_LIBRARIES ${Boost_LIBRARIES})
