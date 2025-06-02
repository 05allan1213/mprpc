set(SRC_LIST calluserservice.cc ../user.pb.cc)

add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer 
    mprpc 
    protobuf
    absl::log
    absl::log_internal_check_impl
    absl::log_internal_conditions
    absl::log_severity
    utf8_range
    absl::status
    absl::statusor
)