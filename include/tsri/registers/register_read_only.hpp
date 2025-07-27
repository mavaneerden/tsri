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
 * @brief TODO:
 *
 * @tparam PeripheralBaseAddress        Base address of the peripheral.
 * @tparam PeripheralBaseAddressOffset  Offest from theh peripheral base address.
 * @tparam ValueOnReset                 Value of the register after the CPU resets.
 * @tparam SupportsAtomicBitOperations  Whether the register supports atomic bit operations (xor, set, clear).
 * @tparam Fields                       Fields inside the register.
 */
template<
    utility::types::register_address_t PeripheralBaseAddress,
    utility::types::register_address_t PeripheralBaseAddressOffset,
    typename... RegisterFields>
class register_read_only : register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>
{
private:
    /* */
    using base_t = register_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>;

    static constexpr auto bit_check_impl(
        const auto&                                result_function,
        const bit_position<RegisterFields...> auto first_bit_position,
        const bit_position<RegisterFields...> auto... bit_positions) -> bool
    {
        const auto bitmask = utility::bit_manipulation::get_bit_positions_bitmask(
            static_cast<utility::types::register_size_t>(first_bit_position),
            static_cast<utility::types::register_size_t>(bit_positions)...);

        /* clang-format off
         * Optimization: if there is only one bit position, we can save some instructions by using only bitwise
         * operations.
         *
         * Example on arm-none-eabi-gcc 14.3.0 with -mthumb and -march=armv6-m:
         * BEFORE:        movs    r3, #255
         *  ands    r0, r3
         *  subs    r3, r3, #254
         *  lsls    r3, r3, r0
         *  ldr     r0, [REGISTER_ADDRESS]
         *  ands    r0, r3
         *  subs    r3, r0, #1
         *  sbcs    r0, r0, r3
         *  uxtb    r0, r0
         * AFTER:
         *  ldr     r3, [REGISTER_ADDRESS]
         *  lsrs    r3, r3, r0
         *  movs    r0, #1
         *  ands    r0, r3
         * clang-format on
         */
        if constexpr (sizeof...(bit_positions) == 0U)
        {
            return (base_t::const_reference() >> first_bit_position) & 1U;
        }

        return result_function(bitmask);
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
    template<bit_position<RegisterFields...> auto... BitPositions>
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
