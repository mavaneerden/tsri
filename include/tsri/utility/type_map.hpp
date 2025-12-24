#pragma once

#include <array>
#include <cstddef>

#include "concepts.hpp"
#include "inline_macro.hpp"
#include "types.hpp"

namespace tsri::utility::types
{

namespace detail
{

/**
 * @brief Implementation of a type map: the keys are types, and they are mapped to values.
 * The keys \b must be unique, and the values must satisfy `std::unsigned_integral`.
 *
 * This type map has optional structured binding support. To enable it, the
 * `TSRI_OPTION_ENABLE_STRUCTURED_BINDING_FOR_GET_FIELDS` macro must be defined.
 *
 * @tparam Keys Keys for the type map, these must be unique.
 */
template<typename... Keys>
    requires concepts::are_types_unqiue<Keys...>
class type_map_impl
{
private:
    /**
     * @brief Get the first index of a type in a parameter pack using recursion.
     *
     * @tparam Type Type to get the index of.
     * @tparam RemainingTypes Remaining types in the pack.
     */
    template<typename Type, typename _, typename... RemainingTypes>
    struct type_pack_index : std::integral_constant<size_t, 1 + type_pack_index<Type, RemainingTypes...>::value>
    {};

    /**
     * @brief Base case for getting the index of a type.
     */
    template<typename Type, typename... RemainingTypes>
    struct type_pack_index<Type, Type, RemainingTypes...> : std::integral_constant<size_t, 0U>
    {};

    /* Map is implemented as an `std::array` because the `Keys` (types) are themselves mapped to unique indices. */
    std::array<register_value_t, sizeof...(Keys)> value_list;

public:
    template<std::unsigned_integral... Values>
        requires (sizeof...(Keys) == sizeof...(Values))
    TSRI_INLINE explicit constexpr type_map_impl(const Values... values) :
        value_list{ values... }
    {}

    type_map_impl()                                        = delete;
    type_map_impl(type_map_impl&&)                         = default;
    type_map_impl(const type_map_impl&)                    = default;
    auto operator=(type_map_impl&&) -> type_map_impl&      = default;
    auto operator=(const type_map_impl&) -> type_map_impl& = default;
    ~type_map_impl()                                       = default;

#ifdef TSRI_OPTION_ENABLE_STRUCTURED_BINDING_FOR_GET_FIELDS
    template<std::size_t Index>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const& noexcept -> auto&&
    {
        return std::get<Index>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const&& noexcept -> auto&&
    {
        return std::get<Index>(value_list);
    }
#endif

protected:
    template<typename Key>
        requires concepts::is_type_in_list<Key, Keys...>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const& noexcept -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }

    template<typename Key>
        requires concepts::is_type_in_list<Key, Keys...>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const&& noexcept -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }
};

}  // namespace detail

template<typename Key, typename... Keys>
class type_map : public detail::type_map_impl<Key, Keys...>
{
    using detail::type_map_impl<Key, Keys...>::type_map_impl;

public:
    template<typename KeyToGet>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const& noexcept -> auto&&
    {
        return detail::type_map_impl<Key, Keys...>::template get<KeyToGet>();
    }

    template<typename KeyToGet>
    [[nodiscard]] TSRI_INLINE constexpr auto get() const&& noexcept -> auto&&
    {
        return detail::type_map_impl<Key, Keys...>::template get<KeyToGet>();
    }
};

template<typename Key>
class type_map<Key> : public detail::type_map_impl<Key>
{
    using detail::type_map_impl<Key>::type_map_impl;

public:
    [[nodiscard]] TSRI_INLINE constexpr auto get() const& noexcept -> auto&&
    {
        return detail::type_map_impl<Key>::template get<Key>();
    }

    [[nodiscard]] TSRI_INLINE constexpr auto get() const&& noexcept -> auto&&
    {
        return detail::type_map_impl<Key>::template get<Key>();
    }
};

}  // namespace tsri::utility::types


#ifdef TSRI_OPTION_ENABLE_STRUCTURED_BINDING_FOR_GET_FIELDS
/* Structured binding support for the type_map class. */
namespace std
{

template<typename... Keys>
struct tuple_size<tsri::utility::types::detail::type_map_impl<Keys...>> :
    public integral_constant<std::size_t, sizeof...(Keys)>
{};

template<std::size_t Index, typename... Keys>
struct tuple_element<Index, tsri::utility::types::detail::type_map_impl<Keys...>>
{
    using type = Keys...[Index]; // TODO: does this work???
};

}  // namespace std
#endif
