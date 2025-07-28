cmake_minimum_required(VERSION 4.0.3)

set(HEADER_DIR "include/tsri")

add_library(project INTERFACE
    ${HEADER_DIR}/fields/field.hpp
    ${HEADER_DIR}/fields/field_types.hpp
    ${HEADER_DIR}/registers/register_base.hpp
    ${HEADER_DIR}/registers/register_read_only.hpp
    ${HEADER_DIR}/registers/register_read_write.hpp
    ${HEADER_DIR}/registers/register_write_base.hpp
    ${HEADER_DIR}/registers/register_write_only.hpp
    ${HEADER_DIR}/utility/bit_manipulation.hpp
    ${HEADER_DIR}/utility/concepts.hpp
    ${HEADER_DIR}/utility/type_map.hpp
    ${HEADER_DIR}/utility/types.hpp
)

target_include_directories(type_safe_register_interface INTERFACE "include")

# TODO: add code generation step
