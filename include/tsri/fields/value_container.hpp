#pragma once

#include <compare>
#include <cstdlib>

#include "../registers/register_read_write.hpp"
#include "../registers/register_write_base.hpp"

namespace tsri::fields
{

template<typename ParentsField>
class value_container
{
    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        utility::types::register_value_t   ValueOnReset,
        typename... RegisterFields>
    friend class registers::register_write_base;

    template<
        utility::types::register_address_t PeripheralBaseAddress,
        utility::types::register_address_t PeripheralBaseAddressOffset,
        utility::types::register_value_t   ValueOnReset,
        bool                               SupportsAtomicBitOperations,
        typename... RegisterFields>
    friend class registers::register_read_write;

private:
    /* Required by register classes to validate if the value's field is settable/readable/etc. */
    using field_t = ParentsField;

    /* Value stored in the container. */
    utility::types::register_value_t stored_value = 0U;

public:
    /**
     * @brief Stores the given value inside the container. `std::unsigned_integral` is used here, because if we used
     * the `register_value_t` directly, it would allow implicit conversions from signed and floating-point numbers.
     * To rule out 64-bit unsigned integers, the value's maximum size is constrained to that of `register_value_t`.
     *
     * @tparam Value Value to store.
     */
    template<std::unsigned_integral Value>
        requires (sizeof(Value) <= sizeof(utility::types::register_value_t))
    TSRI_INLINE explicit constexpr value_container(const Value& value) :
        stored_value(static_cast<utility::types::register_value_t>(value))
    {}

    value_container()                                          = delete;
    value_container(value_container&&)                         = default;
    value_container(const value_container&)                    = default;
    auto operator=(value_container&&) -> value_container&      = default;
    auto operator=(const value_container&) -> value_container& = default;
    ~value_container()                                         = default;

    TSRI_INLINE explicit constexpr operator utility::types::register_value_t() const
    {
        return stored_value;
    }

    TSRI_INLINE constexpr friend auto operator==(
        const value_container& container, const utility::types::register_value_t value) -> bool
    {
        return container.stored_value == value;
    }

    TSRI_INLINE constexpr friend auto operator<=>(
        const value_container& container, const utility::types::register_value_t value) -> std::strong_ordering
    {
        return container.stored_value <=> value;
    }
};

}  // namespace tsri::fields
