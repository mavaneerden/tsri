/**
 * @file register_read_only.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Base class for representation of hardware registers.
 * @version 0.1
 * @date 2025-07-26
 *
 * TODO:
 */
#pragma once

#include "../registers/register_base.hpp"
#include "../utility/bit_manipulation.hpp"
#include "../utility/type_map.hpp"

namespace tsri::registers
{

/**
 * @brief Represents a register where all fields are of the read-only type.
 *
 * @tparam PeripheralBaseAddress        Base address of the peripheral.
 * @tparam PeripheralBaseAddressOffset  Offest from theh peripheral base address.
 * @tparam RegisterFields               Fields inside the register.
 */
template<
    utility::types::register_address_t PeripheralBaseAddress,
    utility::types::register_address_t PeripheralBaseAddressOffset,
    typename... RegisterFields>
class register_read_only : register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>
{
private:
    /* Base class type. Used to access base class static methods. */
    using base_t = register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>;

    /**
     * @brief Implementation function used for the `is_any_bit_set` and `are_all_bits_set` operations.
     *
     * @param evaluate           Function that evaluates the operation. Takes a bitmask of bit positions to check and
     *                           returns a boolean value.
     * @param first_bit_position First bit position, is used for a small optimization (see function body).
     * @param bit_positions      List of bit positions to check.
     * @return true              If the `evaluate` function returns `true`.
     * @return false             If the `evaluate` function returns `false`.
     */
    [[nodiscard]] static constexpr auto bit_check_impl(
        const auto& evaluate, const bit_position<RegisterFields...> auto... bit_positions) -> bool
    {
        const auto bitmask = utility::bit_manipulation::get_bit_positions_bitmask(
            static_cast<utility::types::register_size_t>(bit_positions)...);

        /* clang-format off
         * Optimization: if there is only one bit position, we can save some instructions by using only bitwise
         * operations. We can shift the bit position in the register to the 0 position, and then logical and with 1 to
         * check the bit at the 0 position. This saves a whopping 2 instructions on GCC. Could have been 3 if it didn't
         * do that redundant move... which it only does on -Os for some reason, see https://godbolt.org/z/dr657xPfa
         *
         * Doesn't save any instructions on clang because it already optimizes.
         *
         * Example on arm-none-eabi-gcc 14.3.0 with -Os -mcpu=cortex-m0plus -mfloat-abi=soft -march=armv6-m -mthumb:
         * BEFORE:
         *  movs    r2, #1
         *  lsls    r2, r2, r0
         *  ldr     r3, [REGISTER_ADDRESS]
         *  ands    r3, r2
         *  movs    r0, r3
         *  subs    r3, r0, #1
         *  sbcs    r0, r0, r3
         * AFTER:
         *  ldr     r3, [REGISTER_ADDRESS]
         *  lsrs    r3, r3, r0
         *  movs    r0, r3
         *  movs    r3, #1
         *  ands    r0, r3
         * clang-format on
         */
#if defined(__GNUC__) && !defined(__clang__)
        if constexpr (sizeof...(bit_positions) == 1U)
        {
            return (base_t::const_reference() >> static_cast<utility::types::register_size_t>(bit_positions...[0])) &
                   1U;
        }
#endif

        return evaluate(bitmask);
    }

    /**
     * @brief
     *
     * @param bit_positions
     * @return true
     * @return false
     */
    [[nodiscard]] static constexpr auto is_any_bit_set_impl(const bit_position<RegisterFields...> auto... bit_positions)
        -> bool
    {
        static constexpr auto result_function = [](const auto bitmask) -> bool {
            return (base_t::const_reference() & bitmask) != 0U;
        };

        return bit_check_impl(result_function, bit_positions...);
    }

    /**
     * @brief
     *
     * @param bit_positions
     * @return true
     * @return false
     */
    [[nodiscard]] static constexpr auto are_all_bits_set_impl(
        const bit_position<RegisterFields...> auto... bit_positions) -> bool
    {
        static constexpr auto result_function = [](const auto bitmask) -> bool {
            return (base_t::const_reference() & bitmask) == bitmask;
        };

        return bit_check_impl(result_function, bit_positions...);
    }

public:
    register_read_only()                                             = delete;
    register_read_only(register_read_only&&)                         = delete;
    register_read_only(const register_read_only&)                    = delete;
    auto operator=(register_read_only&&) -> register_read_only&      = delete;
    auto operator=(const register_read_only&) -> register_read_only& = delete;
    ~register_read_only()                                            = delete;

    /**
     * @brief
     *
     * @return utility::types::register_value_t
     */
    [[nodiscard]] static constexpr auto get() -> utility::types::register_value_t
    {
        return base_t::const_reference();
    }

    /**
     * @brief
     *
     * @tparam BitPositions
     * @return true
     * @return false
     */
    template<bit_position_strict<RegisterFields...> auto... BitPositions>
        requires(base_t::template is_bit_position_in_any_readable_field<
                     static_cast<utility::types::register_size_t>(BitPositions)> and
                 ...) and
                utility::concepts::are_values_unique<static_cast<utility::types::register_size_t>(BitPositions)...>
    [[nodiscard]] static constexpr auto is_any_bit_set() -> bool
    {
        return is_any_bit_set_impl(BitPositions...);
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    [[nodiscard]] static constexpr auto is_any_bit_set() -> bool
    {
        return base_t::const_reference() != 0U;
    }

    /**
     * @brief
     *
     * @tparam BitPositions
     * @return true
     * @return false
     */
    template<bit_position<RegisterFields...> auto... BitPositions>
        requires(base_t::template is_bit_position_in_any_readable_field<
                     static_cast<utility::types::register_size_t>(BitPositions)> and
                 ...) and
                utility::concepts::are_values_unique<static_cast<utility::types::register_size_t>(BitPositions)...>
    [[nodiscard]] static constexpr auto are_all_bits_set() -> bool
    {
        return are_all_bits_set_impl(BitPositions...);
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    [[nodiscard]] static constexpr auto are_all_bits_set() -> bool
    {
        static constexpr utility::types::register_value_t all_ones = ~0U;

        return base_t::const_reference() == all_ones;
    }

    /**
     * @brief
     *
     * @note This function uses an optimization which assumes that reserved register bits are always 0. If you get
     * strange values, try turning the optimization off by defining `TSRI_OPTION_NO_OPTIMIZE_GET_FIELDS`.
     *
     * @tparam Fields
     * @return utility::types::type_map
     */
    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...> and
                  base_t::template are_fields_readable<Fields...>)
    [[nodiscard]] static constexpr auto get_fields() -> utility::types::type_map<Fields...>
    {
        /* Register read. */
        const utility::types::register_value_t register_value = base_t::const_reference();

        /* Optimization: if there is only one field in the register, do not use the field bitmask to get its value.
         * This can save one or two instructions, depending on the position of the field in the register.
         *
         * It assumes all other (reserved) register bits are 0, which I think is a fairly safe assumption. But just
         * in case, the optimization can be turned off by defining `TSRI_OPTION_NO_OPTIMIZE_GET_FIELDS`.
         */
#ifndef TSRI_OPTION_NO_OPTIMIZE_GET_FIELDS
        if constexpr (sizeof...(RegisterFields) == 1U)
        {
            return utility::types::type_map<Fields...[0]>{
                Fields...[0] ::get_field_value_from_register_value_no_bitmask(register_value)
            };
        }
#endif

        /* Store the field values in the type map. */
        return utility::types::type_map<Fields...>{ Fields::get_field_value_from_register_value(register_value)... };
    }

    /**
     * @brief
     *
     */
    struct unsafe_operations
    {
        /**
         * @brief
         *
         * @param bit_positions
         * @return true
         * @return false
         */
        [[nodiscard]] static constexpr auto is_any_bit_set(const bit_position<RegisterFields...> auto... bit_positions)
            -> bool
        {
            return is_any_bit_set_impl(bit_positions...);
        }

        /**
         * @brief
         *
         * @param bit_positions
         * @return true
         * @return false
         */
        [[nodiscard]] static constexpr auto are_all_bits_set(
            const bit_position<RegisterFields...> auto... bit_positions) -> bool
        {
            return are_all_bits_set_impl(bit_positions...);
        }
    };
};

}  // namespace tsri::registers
