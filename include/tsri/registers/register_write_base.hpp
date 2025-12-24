/**
 * @file register_write_base.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-26
 *
 * TODO:
 */
#pragma once

#include "../registers/register_base.hpp"

namespace tsri::registers
{

/**
 * @brief TODO:
 *
 * @tparam PeripheralBaseAddress        Base address of the peripheral.
 * @tparam PeripheralBaseAddressOffset  Offest from theh peripheral base address.
 * @tparam ValueOnReset                 Value of the register after the CPU resets.
 * @tparam RegisterFields               Fields inside the register.
 */
template<
    utility::types::register_address_t PeripheralBaseAddress,
    utility::types::register_address_t PeripheralBaseAddressOffset,
    utility::types::register_value_t   ValueOnReset,
    typename... RegisterFields>
class register_write_base : register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>
{
protected:
    using base_t = register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>;

public:
    register_write_base()                                              = delete;
    register_write_base(register_write_base&&)                         = delete;
    register_write_base(const register_write_base&)                    = delete;
    auto operator=(register_write_base&&) -> register_write_base&      = delete;
    auto operator=(const register_write_base&) -> register_write_base& = delete;
    ~register_write_base()                                             = delete;

    // NOLINTBEGIN(readability-redundant-inline-specifier)

    struct unsafe
    {
        /**
        * @brief
        *
        * @param value
        */
        TSRI_INLINE static auto set(const utility::types::register_value_t value) noexcept
        {
            base_t::reference() = value;
        }
    };

    /**
     * @brief
     */
    TSRI_INLINE static auto reset() noexcept
    {
        base_t::reference() = ValueOnReset;
    }

    // NOLINTEND(readability-redundant-inline-specifier)

    /**
     * @brief Set provided fields to the provided values. Overwrites existing register data outside the provied fields
     * with the value on reset. The fields and values are positional: field0 is set to value0, field1 to value1, etc.
     *
     * Equivalent to REG = value1 << shift1 | value2 << shift2 | ... | valueN << shiftN | (~bitmask & value_on_reset);
     *
     * @tparam Values Values to set.
     */
    template<typename... Values>
        requires utility::concepts::are_types_unique_v<typename Values::field_t...> and
                 (base_t::template are_fields_in_register<typename Values::field_t...> and
                  base_t::template are_fields_settable<typename Values::field_t...>)
    TSRI_INLINE static constexpr auto set_fields_overwrite(const Values&... values) noexcept
    {
        /* Reset value needs to be cleared at the field positions. Luckily this can be done at compile-time :) */
        static constexpr auto cleared_reset_value = ~(Values::field_t::bitmask | ...) & ValueOnReset;

        const auto field_values = (Values::field_t::get_register_value_from_field_value(values) | ...);

        base_t::reference() = field_values | cleared_reset_value;
    }

#ifdef __thumb__
    /**
     * @brief Set provided fields to the provided values. Overwrites existing register data outside the provied fields
     * with the value on reset. The fields and values are positional: field0 is set to value0, field1 to value1, etc.
     *
     * This function uses raw assembly STR instructions with immediate offsets to slightly reduce code size when
     * multiple registers from the same peripheral are accessed in sequence. Constants that are normally stored in the
     * binary may be eliminated by using the immediate offset. This works best for registers where the peripheral base
     * address offset <= 124, since the offset is a signed 8-bit value that must be word-aligned. If the peripheral base
     * address offset is > 124, it could reduce code size in fewer cases since a register will be used for the offset
     * instead of an immediate value, though a code size reduction could still occur.
     *
     * @note There is no guarantee that this function actually reduces code size!
     *       Always check the ouputted assembly code.
     *
     * Equivalent to REG = value1 << shift1 | value2 << shift2 | ... | valueN << shiftN | (~bitmask & value_on_reset);
     *
     * @tparam Values Values to set.
     */
    template<typename... Values>
        requires utility::concepts::are_types_unique_v<typename Values::field_t...> and
                 (base_t::template are_fields_in_register<typename Values::field_t...> and
                  base_t::template are_fields_settable<typename Values::field_t...>)
    TSRI_INLINE static constexpr auto set_fields_overwrite_size_optimized(const Values&... values) noexcept
    {
        /* Maximum value of the immediate offset in the store instruction for the Thumb ISA. */
        static constexpr uint32_t isa_offset_max_value = 124U;

        static constexpr auto cleared_reset_value = ~(Values::field_t::bitmask | ...) & ValueOnReset;

        const auto field_values = (Values::field_t::get_register_value_from_field_value(values) | ...);

        const auto register_value_to_set = field_values | cleared_reset_value;

        /* Use a store instruction with immediate offset if the offset fits in the immediate field.
         * Otherwise, use a register as the offset. This is a bit more expensive but can still potentially save some
         * code size.
         */
        // NOLINTNEXTLINE(bugprone-branch-clone): clangd flags this incorrectly.
        if constexpr (PeripheralBaseAddressOffset <= isa_offset_max_value)
        {
            asm volatile("str %[value], [%[base], %[offset]]"
                         :
                         : [value] "r"(register_value_to_set),
                           [base] "r"(PeripheralBaseAddress),
                           [offset] "i"(PeripheralBaseAddressOffset)
                         :);
        }
        else
        {
            asm volatile("str %[value], [%[base], %[offset]]"
                         :
                         : [value] "r"(register_value_to_set),
                           [base] "r"(PeripheralBaseAddress),
                           [offset] "r"(PeripheralBaseAddressOffset)
                         :);
        }
    }
#endif
};

}  // namespace tsri::registers
