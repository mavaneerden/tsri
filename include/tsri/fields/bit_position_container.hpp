#pragma once

#include <concepts>

#include "../utility/inline_macro.hpp"
#include "../utility/types.hpp"

namespace tsri::fields
{

/**
 * @brief Contains a bit position. Used as a strong type wrapper for a bit position value.
 *
 * @tparam ParentField Type of the field that is the parent of this bit position.
 */
template<typename ParentField>
class bit_position_container
{
    /* Field needs to take the bit positions and store them as a bitmask. And since we don't want to expose the
     * stored position to the user, we need to friend the `field` class.
     */
    friend ParentField;

private:
    /**
     * @brief Checks if the bit position container is inside `Field`. We cannot check the stored bit position at
     * compile time, so we assume that the bit position is inside the field, if that field is derived from the
     * template argument.
     * The template argument will always be a `field` type, but we need to use `std::derived_from` since registers
     * will derive their own field types that inherit from `field`.
     *
     * @tparam Field Checks if position container is inside this field.
     */
    template<typename Field>
    static constexpr bool is_bit_position_container_in_field = std::derived_from<Field, ParentField>;

    /* Bit position stored in the container. */
    utility::types::register_size_t stored_bit_position = 0U;

public:
    /**
     * @brief Create a container for the `bit_position`. This position is **relative** to the start bit of the field
     * that the bit position is in! I.e. if a field starts at bit X in a register, then bit position P is position X + P
     * inside the register.
     *
     * This constructor does NOT check if the `bit_position` value actually fits in the field. You should use the
     * field's BIT0, BIT1, etc. if you know beforehand which bit you need; these bits are guaranteed to fit.
     * If the bit you need is only known at runtime, it is your responsibility to check/know that it fits.
     *
     * @tparam BitPosition Type of the position, must be an unsigned integral type.
     * @param bit_position Bit position.
     */
    template<std::unsigned_integral BitPosition>
    TSRI_INLINE explicit constexpr bit_position_container(const BitPosition bit_position) :
        stored_bit_position(bit_position)
    {}

    bit_position_container()                                                 = delete;
    bit_position_container(bit_position_container&&)                         = default;
    bit_position_container(const bit_position_container&)                    = default;
    auto operator=(bit_position_container&&) -> bit_position_container&      = default;
    auto operator=(const bit_position_container&) -> bit_position_container& = default;
    ~bit_position_container()                                                = default;
};

}  // namespace tsri::fields
