/**
 * @file register_base.hpp
 * @author Marco van Eerden (mavaneerden@gmail.com)
 * @brief Base class for representation of hardware registers.
 * @version 0.1
 * @date 2025-07-25
 *
 * This file contains a templated base class for hardware registers.
 * The class serves as a base for the read-only, write-only and read-write registers.
 */
#pragma once

#include <bit>
#include <climits>
#include <concepts>
#include <type_traits>

#include "../utility/concepts.hpp"
#include "../utility/inline_macro.hpp"
#include "../utility/types.hpp"

namespace tsri::registers
{

/**
 * @brief Base class for hardware register representation.
 * Allows derived classes to read from and write to the register and its atomic counterparts (if supported).
 *
 * @tparam PeripheralBaseAddress        Base address of the peripheral.
 * @tparam PeripheralBaseAddressOffset  Offest from theh peripheral base address.
 * @tparam RegisterFields               Fields inside the register.
 */
template<
    utility::types::register_address_t PeripheralBaseAddress,
    utility::types::register_address_t PeripheralBaseAddressOffset,
    typename... RegisterFields>
    requires utility::concepts::are_types_unique_v<RegisterFields...>
class register_base
{
public:
    register_base()                                        = delete;
    register_base(register_base&&)                         = delete;
    register_base(const register_base&)                    = delete;
    auto operator=(register_base&&) -> register_base&      = delete;
    auto operator=(const register_base&) -> register_base& = delete;
    ~register_base()                                       = delete;

private:
    /* Address offset for the atomic XOR on write, see Section 2.1.2 of the RP2040 datasheet. */
    static constexpr utility::types::register_address_t atomic_xor_offset = 0x1000U;
    /* Address offset for the atomic bitmask set on write, see Section 2.1.2 of the RP2040 datasheet. */
    static constexpr utility::types::register_address_t atomic_set_offset = 0x2000U;
    /* Address offset for the atomic bitmask clear on write, see Section 2.1.2 of the RP2040 datasheet. */
    static constexpr utility::types::register_address_t atomic_clear_offset = 0x3000U;

    /* Memory address of the register for normal read/write access. */
    static constexpr utility::types::register_address_t register_address =
        PeripheralBaseAddress + PeripheralBaseAddressOffset;
    /* Memory address of the register's atomic xor on write. */
    static constexpr utility::types::register_address_t register_address_atomic_xor =
        register_address + atomic_xor_offset;
    /* Memory address of the register's atomic bitmask set on write. */
    static constexpr utility::types::register_address_t register_address_atomic_set =
        register_address + atomic_set_offset;
    /* Memory address of the register's atomic bitmask clear on write. */
    static constexpr utility::types::register_address_t register_address_atomic_clear =
        register_address + atomic_clear_offset;

protected:
    /* Checks if the `BitPositionCandidate` corresponds to the bit type of any of the `Fields`,
     * or is an unsigned integer.
     */
    template<typename BitPositionCandidate, typename... Fields>
    static constexpr bool is_bit_position_container_in =
        (std::derived_from<Fields, typename BitPositionCandidate::field_t> or ...);

    template<typename T, typename U>
    struct derived_from_or_same_condition
    {
        static constexpr bool value = std::derived_from<T, U> or std::is_same_v<T, U>;
    };

    /**
     * @brief `true` if `Fields` is a subset of the register's fields, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_in_register =
        (utility::concepts::is_similar_type_in_list_v<derived_from_or_same_condition, Fields, RegisterFields...> and
         ...);

    /**
     * @brief `true` if ALL `Fields` are readable, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_readable = (Fields::is_readable and ...);

    /**
     * @brief `true` if ALL `Fields` are settable, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_settable = (Fields::is_settable and ...);

    /**
     * @brief `true` if ALL `Fields` are clearable, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_clearable = (Fields::is_clearable and ...);

    /**
     * @brief `true` if ALL `Fields` are bit-clearable, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_bit_clearable = (Fields::is_bit_clearable and ...);

    /**
     * @brief `true` if ALL `Fields` are bit-togglable, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_bit_togglable = (Fields::is_bit_togglable and ...);

    /**
     * @brief Returns a mutable reference to the hardware register, which should be used to write to the register in
     * derived classes.
     *
     * @return auto& Mutable reference to the register.
     */
    [[nodiscard]] TSRI_INLINE static auto reference() noexcept -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address);
    }

    /**
     * @brief Returns a immutable reference to the hardware register, which should be used to read from the register in
     * derived classes.
     *
     * @return auto& Immutable reference to the register.
     */
    [[nodiscard]] TSRI_INLINE static auto const_reference() noexcept -> const auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic xor on write, which should be used to write to
     * atomically XOR bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic xor on write register.
     */
    [[nodiscard]] TSRI_INLINE static auto atomic_xor_reference() noexcept -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_xor);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic set bitmask on write, which should be used to
     * write to atomically set bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic set bitmask on write register.
     */
    [[nodiscard]] TSRI_INLINE static auto atomic_set_reference() noexcept -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_set);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic clear bitmask on write, which should be used
     * to write to atomically clear bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic clear bitmask on write register.
     */
    [[nodiscard]] TSRI_INLINE static auto atomic_clear_reference() noexcept -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_clear);
    }

    // NOLINTEND(readability-redundant-inline-specifier)
};

}  // namespace tsri::registers
