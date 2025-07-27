#pragma once

#include <type_traits>

namespace tsri::fields::types
{

struct read_only
{};

struct write_only
{};

struct read_write
{};

struct self_clearing
{};

struct write_clear
{};

template<typename FieldTypeCandidate>
inline constexpr bool is_field_type =
    std::is_same_v<FieldTypeCandidate, read_only> or std::is_same_v<FieldTypeCandidate, write_only> or
    std::is_same_v<FieldTypeCandidate, read_write> or std::is_same_v<FieldTypeCandidate, self_clearing> or
    std::is_same_v<FieldTypeCandidate, write_clear>;

template<typename FieldTypeCandidate>
concept field_type = is_field_type<FieldTypeCandidate>;

template<typename FieldType>
inline constexpr bool is_readable = std::is_same_v<FieldType, read_only> or std::is_same_v<FieldType, read_write> or
                                    std::is_same_v<FieldType, self_clearing>;

/**
 * @brief True for field types that are (bit-)settable by writing a 1.
 * - Read-only is NOT settable because it cannot be written.
 * - Read-write is settable because we can read and write any value.
 * - Write-only is settable because we can write any value.
 * - Self-clearing is settable, because we can write a 1.
 * - Write-clear is settable, because we can write 1 to clear.
 *
 * @tparam FieldType
 */
template<typename FieldType>
inline constexpr bool is_settable = std::is_same_v<FieldType, write_only> or std::is_same_v<FieldType, read_write> or
                                    std::is_same_v<FieldType, self_clearing> or std::is_same_v<FieldType, write_clear>;

/**
 * @brief True for field types that are clearable.
 * - Read-only is NOT clearable because it cannot be written.
 * - Read-write is clearable because we can read and write any value.
 * - Write-only is NOT clearable because its value is always 0.
 * - Self-clearing is NOT clearable because it clears itself.
 * - Write-clear is clearable by writing a 1 to the field.
 *
 * @tparam FieldType
 */
template<typename FieldType>
inline constexpr bool is_clearable = std::is_same_v<FieldType, read_write> or std::is_same_v<FieldType, write_clear>;

/**
 * @brief True for field types that are bit-clearable by writing a 0.
 * - Read-only is NOT bit-clearable because it cannot be written.
 * - Read-write is bit-clearable because we can read and write any value.
 * - Write-only is NOT bit-clearable because its value is always 0.
 * - Self-clearing is NOT bit-clearable because it clears itself.
 * - Write-clear is NOT bit-clearable by writing a 0, we must write a 1.
 *
 * @tparam FieldType
 */
template<typename FieldType>
inline constexpr bool is_bit_clearable = std::is_same_v<FieldType, read_write>;

/**
 * @brief True for field types that are bit-togglable.
 * - Read-only is NOT bit-togglable because it cannot be written.
 * - Read-write is bit-togglable because we can read and write any value.
 * - Write-only is NOT bit-togglable because it has no value (technically its value is 0, but toggle would always be 1).
 * - Self-clearing is NOT bit-togglable because only 1 can be written.
 * - Write-clear is NOT bit-togglable because only 1 can be written.
 *
 * @tparam FieldType
 */
template<typename FieldType>
inline constexpr bool is_bit_togglable = std::is_same_v<FieldType, read_write>;

}  // namespace tsri::fields::types
