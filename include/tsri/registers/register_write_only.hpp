/**
 * @file register_write_only.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-26
 *
 * TODO:
 */
#pragma once

#include "../registers/register_write_base.hpp"
#include "../utility/bit_manipulation.hpp"

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
    utility::types::register_value_t   ValueOnReset,
    bool                               SupportsAtomicBitOperations,
    typename... RegisterFields>
class register_write_only :
    public register_write_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, ValueOnReset, RegisterFields...>
{
private:
    /* */
    using write_base_t =
        register_write_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, ValueOnReset, RegisterFields...>;

    /**
     * @brief
     *
     * @param bit_positions
     * @return
     */
    static constexpr auto set_bits_impl(const bit_position<RegisterFields...> auto... bit_positions)
    {
        static constexpr auto bitmask = utility::bit_manipulation::get_bit_positions_bitmask(
            static_cast<utility::types::register_size_t>(bit_positions)...);

        write_base_t::reference() = bitmask;
    }

public:
    register_write_only()                                              = delete;
    register_write_only(register_write_only&&)                         = delete;
    register_write_only(const register_write_only&)                    = delete;
    auto operator=(register_write_only&&) -> register_write_only&      = delete;
    auto operator=(const register_write_only&) -> register_write_only& = delete;
    ~register_write_only()                                             = delete;

    /**
     * @brief
     *
     * @tparam BitPositions
     */
    template<bit_position<RegisterFields...> auto... BitPositions>
        requires(write_base_t::template is_bit_position_in_any_write_only_field<
                     static_cast<utility::types::register_size_t>(BitPositions)> and
                 ...) and
                utility::concepts::are_values_unique<static_cast<utility::types::register_size_t>(BitPositions)...>
    static constexpr auto set_bits()
    {
        set_bits_impl(BitPositions...);
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
         */
        static constexpr auto set_bits(const bit_position<RegisterFields...> auto... bit_positions)
        {
            set_bits_impl(bit_positions...);
        }
    };
};

}  // namespace tsri::registers
