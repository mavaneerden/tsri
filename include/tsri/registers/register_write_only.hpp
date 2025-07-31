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

public:
    register_write_only()                                              = delete;
    register_write_only(register_write_only&&)                         = delete;
    register_write_only(const register_write_only&)                    = delete;
    auto operator=(register_write_only&&) -> register_write_only&      = delete;
    auto operator=(const register_write_only&) -> register_write_only& = delete;
    ~register_write_only()                                             = delete;

    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (write_base_t::template are_fields_in_register<Fields...>)
    /* No need for field check here: all fields are write-only. */
    TSRI_INLINE static constexpr auto set_bits(const Fields&&... fields) noexcept
    {
        write_base_t::reference() = (fields.stored_bitmask | ...);
    }
};

}  // namespace tsri::registers
