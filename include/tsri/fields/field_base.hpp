/**
 * @file field_base.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Base class for the representation of hardware register fields.
 * @version 0.1
 * @date 2025-07-25
 *
 * TODO:
 */
#pragma once

#include <concepts>
#include <type_traits>

#include "../registers/register_read_write.hpp"
#include "../registers/register_write_only.hpp"
#include "../utility/types.hpp"
#include "field_types.hpp"

namespace tsri::fields
{

/**
 * @brief Base class for hardware register field representation.
 * Derived classes should specialise on access modifier: read-only, write-only or read-write.
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
    types::field_type               FieldType,
    std::semiregular                ValueType,
    typename BitType,
    ValueType FieldValueOnReset>
    requires std::is_scoped_enum_v<BitType>
class field_base
{
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

public:
    field_base()                                     = delete;
    field_base(field_base&&)                         = delete;
    field_base(const field_base&)                    = delete;
    auto operator=(field_base&&) -> field_base&      = delete;
    auto operator=(const field_base&) -> field_base& = delete;
    ~field_base()                                    = delete;

    /* */
    using value_t = ValueType;

    /* */
    using bit_t = BitType;

    /* */
    static constexpr value_t value_on_reset = FieldValueOnReset;

protected:
    /* Whether the field is readable. */
    static constexpr bool is_readable = types::is_readable<FieldType>;

    /* Whether field can be set. */
    static constexpr bool is_settable = types::is_settable<FieldType>;

    /* Whether field can be cleared. */
    static constexpr bool is_clearable = types::is_clearable<FieldType>;

    /* Whether field can be cleared on the bit level. */
    static constexpr bool is_bit_clearable = types::is_bit_clearable<FieldType>;

    /* Whether field can be toggled on the bit level. */
    static constexpr bool is_bit_togglable = types::is_bit_togglable<FieldType>;

    /* Whether field can be toggled on the bit level. */
    static constexpr bool is_write_clear = std::is_same_v<FieldType, types::write_clear>;

    /**
     * @brief
     *
     * @tparam BitPosition
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_field = BitPosition >= StartBit and BitPosition < (LengthInBits + StartBit);

    /* Bitmask of the field inside the register. */
    static constexpr auto bitmask = []() -> utility::types::register_value_t {
        /* BITS will contain all 1s. */
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
    static constexpr auto get_register_value_from_field_value(const ValueType value) -> utility::types::register_value_t
    {
        return (static_cast<utility::types::register_value_t>(value) << StartBit) & bitmask;
    }

    /**
     * @brief
     *
     * @param value
     * @return ValueType
     */
    static constexpr auto get_field_value_from_register_value(const utility::types::register_value_t value) -> ValueType
    {
        return static_cast<ValueType>((value & bitmask) >> StartBit);
    }
};

}  // namespace tsri::fields
