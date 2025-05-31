aux_source_directory(./ SRC_LIST)
add_library(mprpc SHARED ${SRC_LIST})

target_link_libraries(mprpc
    protobuf
    absl::log
    absl::log_internal_check_impl
    absl::log_internal_conditions
    absl::log_severity
    absl::flat_hash_map
    absl::flat_hash_set
    absl::raw_hash_map
    absl::raw_hash_set
)