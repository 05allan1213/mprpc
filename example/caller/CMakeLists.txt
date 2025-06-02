aux_source_directory(./src SRC)

add_executable(caller ${SRC})

target_link_libraries(caller 
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