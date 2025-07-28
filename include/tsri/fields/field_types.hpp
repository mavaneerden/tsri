/**
 * @file field_types.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Decribes the possible types of fields.
 * @version 0.1
 * @date 2025-07-25
 *
 * Each field has a "field type", which says something about how the field can be accessed and manipulated.
 *
 * There are five field types:
 * 1. `read_only`: for fields that may only be read.
 * 2. `write_only`: for fields that may only be written. A read on this field yields 0.
 * 3. `read_write`: any read or write operation may be performed.
 * 4. `self_clearing`: when written with value `1`, clears itself after an event has triggered. It can be read to
 *    monitor the event's status.
 * 5. `write_clear`: when written with value `1`, clears itself immediately. Reading the register works as normal.
 *
 * In addition to the field types, this file defines some categories of types, listed below. For more detailed
 * information, refer to the documentation of the specific category.
 * - `is_readable`: contains all field types that are readable (`read_only`, `read_write`, `self_clearing`,
 *   `write_clear`);
 * - `is_settable`: contains all field types that are writable (`write_only`, `read_write`, `self_clearing`,
 *   `write_clear`);
 * - `is_clearable`: contains all field types that are clearable (`read_write`, `write_clear`);
 * - `is_bit_clearable`: contains all field types where individual bits can be cleared (`read_write`);
 * - `is_bit_togglable`: contains all field types where individual bits can be toggled (`read_write`).
 */
#pragma once

#include <type_traits>

namespace tsri::fields::field_types
{

/* Read-only fields can only be read. Duh. */
struct read_only
{};

/* Write-only fields can only be set. */
struct write_only
{};

/* Read-write fields can be read, set, cleared, bit-cleared and bit-toggled. */
struct read_write
{};

/* Self-clearing fields can be read or set. */
struct self_clearing
{};

/* Write-clear fields can be read, set or cleared. */
struct write_clear
{};

/**
 * @brief Checks if the given `FieldTypeCandidate` type is a field type.
 */
template<typename FieldTypeCandidate>
concept field_type =
    std::is_same_v<FieldTypeCandidate, read_only> or std::is_same_v<FieldTypeCandidate, write_only> or
    std::is_same_v<FieldTypeCandidate, read_write> or std::is_same_v<FieldTypeCandidate, self_clearing> or
    std::is_same_v<FieldTypeCandidate, write_clear>;

/**
 * @brief True for field types that are readable.
 * - Read-only is readable because it can be read.
 * - Read-write is readable because it can be read.
 * - Write-only is NOT readable because its value is always 0.
 * - Self-clearing is readable because it can be read.
 * - Write-clear NOT readable because it can only be written.
 *
 * @tparam FieldType Field type to check.
 */
template<typename FieldType>
inline constexpr bool is_readable = std::is_same_v<FieldType, read_only> or std::is_same_v<FieldType, read_write> or
                                    std::is_same_v<FieldType, self_clearing> or std::is_same_v<FieldType, write_clear>;

/**
 * @brief True for field types that are (bit-)settable by writing a 1.
 * - Read-only is NOT settable because it cannot be written.
 * - Read-write is settable because we can read and write any value.
 * - Write-only is settable because we can write any value.
 * - Self-clearing is settable, because we can write a 1.
 * - Write-clear is settable, because we can write 1 to clear.
 *
 * @tparam FieldType Field type to check.
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
 * @tparam FieldType Field type to check.
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
 * @tparam FieldType Field type to check.
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
 * @tparam FieldType Field type to check.
 */
template<typename FieldType>
inline constexpr bool is_bit_togglable = std::is_same_v<FieldType, read_write>;

}  // namespace tsri::fields::field_types
