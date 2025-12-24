/**
 * @file register_read_only.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Base class for read-only registers.
 * @version 0.1
 * @date 2025-07-26
 *
 * The `register_read_only` class can be inherited from to create a read-only register.
 */
#pragma once

#include "../registers/register_base.hpp"
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

public:
    register_read_only()                                             = delete;
    register_read_only(register_read_only&&)                         = delete;
    register_read_only(const register_read_only&)                    = delete;
    auto operator=(register_read_only&&) -> register_read_only&      = delete;
    auto operator=(const register_read_only&) -> register_read_only& = delete;
    ~register_read_only()                                            = delete;

    /**
     * @brief TODO:
     *
     * @return utility::types::register_value_t
     */
    [[nodiscard]] TSRI_INLINE static auto get() noexcept -> utility::types::register_value_t
    {
        return base_t::const_reference();
    }

    /**
     * @brief TODO:
     *
     * @return true
     * @return false
     */
    [[nodiscard]] TSRI_INLINE static constexpr auto is_any_bit_set() noexcept -> bool
    {
        return base_t::const_reference() != 0U;
    }

    /**
     * @brief TODO:
     *
     * @return true
     * @return false
     */
    [[nodiscard]] TSRI_INLINE static constexpr auto are_all_bits_set() noexcept -> bool
    {
        static constexpr utility::types::register_value_t all_ones = ~0U;

        return base_t::const_reference() == all_ones;
    }

    /**
     * @brief TODO:
     *
     * @note This function uses an optimization which assumes that reserved register bits are always 0. If you get
     * strange values, try turning the optimization off by defining `TSRI_OPTION_NO_OPTIMIZE_GET_FIELDS`.
     *
     * @tparam Fields Fields to get values from.
     * @return utility::types::type_map
     */
    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_readable<Fields...>)
    [[nodiscard]] TSRI_INLINE static constexpr auto get_fields() noexcept -> utility::types::type_map<Fields...>
    {
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
            // TODO: use C++26 pack indexing when compilers have matured
            return utility::types::type_map<Fields...>{ Fields::get_field_value_from_register_value_no_bitmask(
                register_value)... };
        }
#endif

        return utility::types::type_map<Fields...>{ Fields::get_field_value_from_register_value(register_value)... };
    }

    /**
     * @brief TODO:
     *
     * @tparam Fields
     */
    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_readable<Fields...>)
    [[nodiscard]] TSRI_INLINE static constexpr auto is_any_bit_set(const Fields&&... fields) noexcept -> bool
    {
        const auto bitmask = (fields.stored_bitmask | ...);

        return (base_t::const_reference() & bitmask) != 0U;
    }

    /**
     * @brief TODO:
     *
     * @tparam Fields
     */
    template<typename... Fields>
        requires utility::concepts::are_types_unique_v<Fields...> and
                 (base_t::template are_fields_in_register<Fields...>) and
                 (base_t::template are_fields_readable<Fields...>)
    [[nodiscard]] TSRI_INLINE static constexpr auto are_all_bits_set(const Fields&&... fields) noexcept -> bool
    {
        const auto bitmask = (fields.stored_bitmask | ...);

        return (base_t::const_reference() & bitmask) == bitmask;
    }
};

}  // namespace tsri::registers
