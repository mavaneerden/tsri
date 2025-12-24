/**
 * @file field.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Contains a class for the representation of hardware register fields.
 * @version 0.1
 * @date 2025-07-25
 */
#pragma once

#include <climits>
#include <concepts>
#include <type_traits>

// #include "../registers/register_read_write.hpp"
#include "../registers/register_write_only.hpp"
#include "../utility/types.hpp"
#include "bit_position_container.hpp"
#include "field_types.hpp"
#include "value_container.hpp"

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
 * @tparam TypeOfField  Access type for the field.
 * @tparam FieldValueOnReset Reset value of the field.
 * @tparam RegisterAddress Address of the register that the field belongs to. Used to make each field type unique.
 */
template<
    utility::types::register_size_t    StartBit,
    utility::types::register_size_t    LengthInBits,
    field_types::field_type            TypeOfField,
    utility::types::register_value_t   FieldValueOnReset,
    utility::types::register_address_t RegisterAddress>
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
    using this_t = field<StartBit, LengthInBits, TypeOfField, FieldValueOnReset, RegisterAddress>;

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

protected:
    using bit = bit_position_container<this_t>;

    using value = value_container<this_t>;

private:
    /**
     * @brief Bitmask of bit positions.
     */
    utility::types::register_value_t stored_bitmask;

public:
    /* Value of the field after processor reset. */
    static constexpr value reset_value{ FieldValueOnReset };

    /* Clear value of the register. If it is write-clear, we must write a 1 to clear the field. Otherwise we can write 0
     * or any value, so just stick to 0.
     */
    static constexpr value clear_value{ is_write_clear ? 1U : 0U };

    /**
     * @brief Takes bit position containers (of type bit) and converts their bit positions into a bitmask at the field
     * position in the register.
     * This constructor is used in the bit manipulation/access functions in order to group bit positions by field.
     *
     * @tparam BitPositionContainer Bit position containers.
     */
    template<typename... BitPositionContainer>
        requires (BitPositionContainer::template is_bit_position_container_in_field<this_t> and ...)
    TSRI_INLINE constexpr explicit field(const BitPositionContainer&... containers) :
        stored_bitmask(this_t::get_bitmask_from_bit_positions(containers.stored_bit_position...))
    {}

    field()                                = delete;
    field(field&&)                         = default;
    field(const field&)                    = default;
    auto operator=(field&&) -> field&      = default;
    auto operator=(const field&) -> field& = default;
    ~field()                               = default;

private:
    /* Bitmask of the field inside the register. */
    static constexpr auto bitmask = []() -> utility::types::register_value_t {
        static constexpr utility::types::register_value_t one_bits = ~0U;

        /**
         * Right shift is done to get the correct \em number of bits required for the mask.
         * For example, if we want the mask 00111000, the right shift would be 8 - 3 = 5:
         * 11111111 >> 5 = 00000111
         */
        static constexpr utility::types::register_size_t right_shift =
            (sizeof(utility::types::register_value_t) * 8U) - LengthInBits;

        /**
         * Left shift is done to put the number of bits acquired from the right shift in the correct \em position.
         * For example, if we want the mask 00111000, left shift would be 3 (equal to the start bit position):
         * 11111111 >> 5 = 00000111;
         * 00000111 << 3 = 00111000
         */
        return (one_bits >> right_shift) << StartBit;
    }();

    /**
     * @brief Return the given value shifted and bitmasked to the field's position in its register.
     * Can be used in consteval context.
     *
     * @param value Value to insert into the field's position in its register.
     * @return utility::types::register_value_t Value shifted and bitmasked into the field's position.
     */
    TSRI_INLINE static constexpr auto get_register_value_from_field_value(const value& value) noexcept
        -> utility::types::register_value_t
    {
        return (static_cast<utility::types::register_value_t>(value) << StartBit) & bitmask;
    }

    /**
     * @brief Extract the field's value from the value of the register. I.e. use bitmask and shift to the 0 position.
     *
     * @param value Register value.
     * @return utility::types::register_value_t Field value.
     */
    TSRI_INLINE static constexpr auto get_field_value_from_register_value(
        const utility::types::register_value_t value) noexcept -> utility::types::register_value_t
    {
        return (value & bitmask) >> StartBit;
    }

    /**
     * @brief Extract the field's value from the value of the register, but do not use a bitmask.
     * This function can be used if the field is the only field in the register.
     *
     * @param value Register value.
     * @return utility::types::register_value_t Field value..
     */
    TSRI_INLINE static constexpr auto get_field_value_from_register_value_no_bitmask(
        const utility::types::register_value_t value) noexcept -> utility::types::register_value_t
    {
        return value >> StartBit;
    }

    /**
     * @brief Get the bit mask from bit positions in the field, shifted to the field position.
     * The bit positions start at 0.
     *
     * @param bit_positions List of bit positions.
     * @return utility::types::register_value_t
     */
    TSRI_INLINE static constexpr auto get_bitmask_from_bit_positions(const std::unsigned_integral auto... bit_positions)
        -> utility::types::register_value_t
    {
        return ((1U << bit_positions) | ...) << StartBit;
    }
};

}  // namespace tsri::fields
