/**
 * @file field.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Contains a class for the representation of hardware register fields.
 * @version 0.1
 * @date 2025-07-25
 */
#pragma once

#include <array>
#include <concepts>
#include <type_traits>
#include <utility>

#include "../registers/register_read_write.hpp"
#include "../registers/register_write_only.hpp"
#include "../utility/types.hpp"
#include "field_types.hpp"

namespace tsri::fields
{

/**
 * @brief Class for hardware register field representation.
 * All register classes need to make use of the fields, but we don't want to expose the user to all of its functions.
 * As such, the `field` class must befriend all register classes.
 *
 * This class exposes a grand total of three (3) things:
 *  1. `value_t`: the type of the field value. If it is an enum, this can be used to access its values.
 *  2. `bit_t`: the type of the field bits, this should be an `enum class`.
 *  3. `value_on_reset`: default value of the field after the processor resets. Can be used for e.g. setting the field
 *     back to its reset value.
 *
 * @tparam StartBit     Start bit position in the register.
 * @tparam LengthInBits Length of the field in bits
 * @tparam ValueType    Type of the field value. Can be any built-in type or (scoped) enum.
 * @tparam BitType      Enumeration type with bit positions corresponding to the positions of the field's bits inside
 *                      the register.
 */
template<
    utility::types::register_size_t StartBit,
    utility::types::register_size_t LengthInBits,
    field_types::field_type         TypeOfField,
    std::semiregular                ValueType,
    typename BitType,
    ValueType FieldValueOnReset>
    requires std::is_scoped_enum_v<BitType>
class field
{
    /* Ayo this class has more friends than me... 🥲 */

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        typename... RegisterFields>
        requires utility::concepts::are_types_unique_v<RegisterFields...>
    friend class registers::register_base;

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        typename... RegisterFields>
    friend class registers::register_read_only;

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        utility::types::register_value_t   ValueOnReset,
        typename... RegisterFields>
    friend class registers::register_write_base;

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        utility::types::register_value_t   ValueOnReset,
        bool                               SupportsAtomicBitOperations,
        typename... RegisterFields>
    friend class registers::register_write_only;

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        utility::types::register_value_t   ValueOnReset,
        bool                               SupportsAtomicBitOperations,
        typename... RegisterFields>
    friend class registers::register_read_write;

private:
    /**
     * @brief True if the given `ValueCandidate` type is considered a field value type. It is so if:
     * - The field `ValueType` is a scoped enum and `ValueCandidate` is that type OR
     * - The field `ValueType` is `bool` and `ValueCandidate` is `bool` OR
     * - `ValueCandidate` is convertible to `ValueType` and unsigned integral.
     *
     * @tparam ValueCandidate Type to check.
     */
    template<typename ValueCandidate>
    static constexpr bool is_field_value =
        (std::is_enum_v<ValueType> or std::is_same_v<ValueType, bool>)
            ? std::is_same_v<ValueCandidate, ValueType>
            : (std::is_convertible_v<ValueCandidate, ValueType> and std::unsigned_integral<ValueCandidate>);

    ValueType value_internal;

public:
    /* Type of the field value. If this is a (scoped) enum, this can be used to get the enumeration values. */
    using value = ValueType;

    /* Field bit type, can be used to get the individual field bit positions in the register. */
    using bit = BitType;

    /* Value of the field after processor reset. */
    static constexpr ValueType value_on_reset = FieldValueOnReset;

    template<typename ValueArgType>
        requires is_field_value<ValueArgType>
    constexpr explicit field(const ValueArgType value) :
        value_internal(static_cast<ValueType>(value))
    {}

    field()                                = delete;
    field(field&&)                         = default;
    field(const field&)                    = default;
    auto operator=(field&&) -> field&      = default;
    auto operator=(const field&) -> field& = default;
    ~field()                               = default;

private:
    /* Whether the field is readable. */
    static constexpr bool is_readable = field_types::is_readable<TypeOfField>;

    /* Whether field can be set. */
    static constexpr bool is_settable = field_types::is_settable<TypeOfField>;

    /* Whether field can be cleared. */
    static constexpr bool is_clearable = field_types::is_clearable<TypeOfField>;

    /* Whether field can be cleared on the bit level. */
    static constexpr bool is_bit_clearable = field_types::is_bit_clearable<TypeOfField>;

    /* Whether field can be toggled on the bit level. */
    static constexpr bool is_bit_togglable = field_types::is_bit_togglable<TypeOfField>;

    /* Whether field can be toggled on the bit level. */
    static constexpr bool is_write_clear = std::is_same_v<TypeOfField, field_types::write_clear>;

    /**
     * @brief True if the given bit position lies inside the field, false otherwise.
     *
     * @tparam BitPosition Bit position to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_field =
        BitPosition >= StartBit and BitPosition < (LengthInBits + StartBit);

    /* Bitmask of the field inside the register. */
    static constexpr auto bitmask = []() -> utility::types::register_value_t {
        static constexpr utility::types::register_value_t one_bits = ~0U;

        /**
         * Right shift is done to get the correct \em number of bits required for the mask.
         * For example, if we want the mask 00111000, the right shift would be 8 - 3 = 5:
         * 11111111 >> 5 = 00000111
         */
        static constexpr utility::types::register_size_t right_shift = registers::num_bits_in_register - LengthInBits;

        /**
         * Left shift is done to put the number of bits acquired from the right shift in the correct \em position.
         * For example, if we want the mask 00111000, left shift would be 3 (equal to the start bit position):
         * 11111111 >> 5 = 00000111;
         * 00000111 << 3 = 00111000
         */
        return (one_bits >> right_shift) << StartBit;
    }();

    /* Clear value of the register. If it is write-clear, we must write a 1 to clear the field. Otherwise we can write 0
     * or any value, so just stick to 0.
     */
    static constexpr utility::types::register_value_t clear_value = is_write_clear ? 1U : 0U;

    /**
     * @brief Return the given value shifted and bitmasked to the field's position in its register.
     * Can be used in consteval context.
     *
     * @param value Value to insert into the field's position in its register.
     * @return utility::types::register_value_t Value shifted and bitmasked into the field's position.
     */
    static constexpr auto get_register_value_from_field_value(const ValueType value) noexcept
        -> utility::types::register_value_t
    {
        return (static_cast<utility::types::register_value_t>(value) << StartBit) & bitmask;
    }

    /**
     * @brief Extract the field's value from the value of the register. I.e. use bitmask and shift to the 0 position.
     *
     * @param value Register value.
     * @return ValueType Field value, of type value_t.
     */
    static constexpr auto get_field_value_from_register_value(const utility::types::register_value_t value) noexcept
        -> ValueType
    {
        return static_cast<ValueType>((value & bitmask) >> StartBit);
    }

    /**
     * @brief Extract the field's value from the value of the register, but do not use a bitmask.
     * This function can be used if the field is the only field in the register.
     *
     * @param value Register value.
     * @return ValueType Field value, of type value_t.
     */
    static constexpr auto get_field_value_from_register_value_no_bitmask(
        const utility::types::register_value_t value) noexcept -> ValueType
    {
        return static_cast<ValueType>((value) >> StartBit);
    }
};

}  // namespace tsri::fields
