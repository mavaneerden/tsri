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
#include "../utility/types.hpp"

namespace tsri::registers
{

/* Checks if the `BitPositionCandidate` corresponds to the bit type of any of the `Fields`. */
template<typename BitPositionCandidate, typename... Fields>
concept bit_position_strict = (std::is_same_v<BitPositionCandidate, typename Fields::bit_t> or ...);

/* Checks if the `BitPositionCandidate` corresponds to the bit type of any of the `Fields`,
 * or is an unsigned integer.
 */
template<typename BitPositionCandidate, typename... Fields>
concept bit_position = (std::is_same_v<BitPositionCandidate, typename Fields::bit_t> or ...) or
                       std::unsigned_integral<BitPositionCandidate>;

/* Number of bits in a register. */
inline constexpr utility::types::register_size_t num_bits_in_register =
    sizeof(utility::types::register_value_t) * CHAR_BIT;

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
    /**
     * @brief `true` if `Fields` is a subset of the register's fields, `false` otherwise.
     *
     * @tparam Fields Fields to check.
     */
    template<typename... Fields>
    static constexpr bool are_fields_in_register =
        (utility::concepts::is_type_in_list_v<Fields, RegisterFields...> and ...);

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
     * @brief `true` if the `BitPosition` is inside any readable field in the register, `false` otherwise.
     *
     * @tparam BitPosition Bit positoin to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_any_readable_field =
        ((RegisterFields::template is_bit_position_in_field<BitPosition> and RegisterFields::is_readable) or ...);

    /**
     * @brief `true` if the `BitPosition` is inside any write-only field in the register, `false` otherwise.
     *
     * @tparam BitPosition Bit positoin to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_any_write_only_field =
        ((RegisterFields::template is_bit_position_in_field<BitPosition> and RegisterFields::is_write_only) or ...);

    /**
     * @brief `true` if the `BitPosition` is inside any settable field in the register, `false` otherwise.
     *
     * @tparam BitPosition Bit positoin to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_any_settable_field =
        ((RegisterFields::template is_bit_position_in_field<BitPosition> and RegisterFields::is_settable) or ...);

    /**
     * @brief `true` if the `BitPosition` is inside any bit-clearable field in the register, `false` otherwise.
     *
     * @tparam BitPosition Bit positoin to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_any_bit_clearable_field =
        ((RegisterFields::template is_bit_position_in_field<BitPosition> and RegisterFields::is_bit_clearable) or ...);

    /**
     * @brief `true` if the `BitPosition` is inside any bit-togglable field in the register, `false` otherwise.
     *
     * @tparam BitPosition Bit positoin to check.
     */
    template<utility::types::register_size_t BitPosition>
    static constexpr bool is_bit_position_in_any_bit_togglable_field =
        ((RegisterFields::template is_bit_position_in_field<BitPosition> and RegisterFields::is_bit_togglable) or ...);

    /* NOLINTBEGIN(readability-redundant-inline-specifier)
     * Inline is actually not redundant here. On GCC with -Og, these functions are not inlined without the `inline` or
     * `constexpr` specifier. Constexpr would work, but in my opinion it is not the correct keyword to use because these
     * functions can never be evaluated at compile time: it is a cast to a volatile pointer.
     */

    /**
     * @brief Returns a mutable reference to the hardware register, which should be used to write to the register in
     * derived classes.
     *
     * @return auto& Mutable reference to the register.
     */
    [[nodiscard]] static inline auto reference() -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address);
    }

    /**
     * @brief Returns a immutable reference to the hardware register, which should be used to read from the register in
     * derived classes.
     *
     * @return auto& Immutable reference to the register.
     */
    [[nodiscard]] static inline auto const_reference() -> const auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic xor on write, which should be used to write to
     * atomically XOR bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic xor on write register.
     */
    [[nodiscard]] static inline auto atomic_xor_reference() -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_xor);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic set bitmask on write, which should be used to
     * write to atomically set bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic set bitmask on write register.
     */
    [[nodiscard]] static inline auto atomic_set_reference() -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_set);
    }

    /**
     * @brief Returns a mutable reference to the hardware register atomic clear bitmask on write, which should be used
     * to write to atomically clear bits in the register in derived classes.
     *
     * @return auto& Mutable reference to the atomic clear bitmask on write register.
     */
    [[nodiscard]] static inline auto atomic_clear_reference() -> auto&
    {
        return *std::bit_cast<utility::types::register_ptr_t>(register_address_atomic_clear);
    }

    // NOLINTEND(readability-redundant-inline-specifier)
};

}  // namespace tsri::registers
