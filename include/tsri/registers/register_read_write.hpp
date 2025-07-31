/**
 * @file register_read_write.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-26
 *
 * TODO:
 */
#pragma once

#include "../registers/register_read_only.hpp"
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
class register_read_write :
    public register_write_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, ValueOnReset, RegisterFields...>,
    public register_read_only<PeripheralBaseAddress, PeripheralBaseAddressOffset, RegisterFields...>
{
private:
    using base_t =
        register_write_base<PeripheralBaseAddress, PeripheralBaseAddressOffset, ValueOnReset, RegisterFields...>::
            base_t;

public:
    register_read_write()                                              = delete;
    register_read_write(register_read_write&&)                         = delete;
    register_read_write(const register_read_write&)                    = delete;
    auto operator=(register_read_write&&) -> register_read_write&      = delete;
    auto operator=(const register_read_write&) -> register_read_write& = delete;
    ~register_read_write()                                             = delete;

    /**
     * @brief Set provided fields to the provided values. Does not overwrite existing register data.
     * Equivalent to REG = value1 << shift1 | value2 << shift2 | ... | valueN << shiftN | (~bitmask & REG);
     *
     * @tparam Fields Fields to set.
     */
    template<typename... Values>
        requires utility::concepts::are_types_unique_v<typename Values::field_t...> and
                 (base_t::template are_fields_in_register<typename Values::field_t...> and
                  base_t::template are_fields_settable<typename Values::field_t...>)
    TSRI_INLINE static constexpr auto set_fields(const Values&... values) noexcept
    {
        /* Register value needs to be cleared at the field positions. */
        const auto cleared_register_value = ~(Values::field_t::bitmask | ...) & base_t::const_reference();

        const auto field_values = (Values::field_t::get_register_value_from_field_value(values) | ...);

        base_t::reference() = field_values | cleared_register_value;
    }

    /**
     * @brief Clears the given fields.
     * The clear is done using the atomic clear register, if it is supported.
     * If at least one of the given fields is a write-clear (WC) field, the clear is done using the regular register,
     * whether atomic clear is supported or not (since atomic clears don't work on WC fields).
     *
     * @note Only works for read-write and write-clear fields. For write-only fields, use set_fields() instead.
     *
     * @tparam Fields Fields to clear.
     */
    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...> and
                  base_t::template are_fields_clearable<Fields...>)
    TSRI_INLINE static constexpr auto clear_fields() noexcept
    {
        static constexpr auto fields_bitmask = (Fields::bitmask | ...);

        if constexpr (SupportsAtomicBitOperations and !(Fields::is_write_clear or ...))
        {
            base_t::atomic_clear_reference() = fields_bitmask;
        }
        else
        {
            /* Get the combined clear value of all the fields. If there is no write-clear field, this value will
             * be 0 and is optimized away. Else, there will be one or more 1-bits in there and we require an extra
             * OR operation.
             */
            static constexpr auto fields_clear_value =
                (Fields::get_register_value_from_field_value(static_cast<Fields::value>(Fields::clear_value)) | ...);

            base_t::reference() = (~fields_bitmask & base_t::const_reference()) | fields_clear_value;
        }
    }

    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_settable<Fields...>)
    TSRI_INLINE static constexpr auto set_bits(const Fields&&... fields) noexcept
    {
        const auto bitmask = (fields.stored_bitmask | ...);

        if constexpr (SupportsAtomicBitOperations)
        {
            base_t::atomic_set_reference() = bitmask;
        }
        else
        {
            base_t::reference() = bitmask | base_t::const_reference();
        }
    }

    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_bit_clearable<Fields...>)
    TSRI_INLINE static constexpr auto clear_bits(const Fields&&... fields) noexcept
    {
        const auto bitmask = (fields.stored_bitmask | ...);

        if constexpr (SupportsAtomicBitOperations)
        {
            base_t::atomic_clear_reference() = bitmask;
        }
        else
        {
            base_t::reference() = ~bitmask & base_t::const_reference();
        }
    }

    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_bit_togglable<Fields...>)
    TSRI_INLINE static constexpr auto toggle_bits(const Fields&&... fields) noexcept
    {
        const auto bitmask = (fields.stored_bitmask | ...);

        if constexpr (SupportsAtomicBitOperations)
        {
            base_t::atomic_xor_reference() = bitmask;
        }
        else
        {
            base_t::reference() = bitmask ^ base_t::const_reference();
        }
    }
};

}  // namespace tsri::registers
