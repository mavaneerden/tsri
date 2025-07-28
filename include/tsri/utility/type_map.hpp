#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "concepts.hpp"

namespace tsri::utility::types
{

template<typename... Keys>
    requires concepts::are_types_unqiue<Keys...>
class type_map
{
private:
    // helper type to get index of T in pack Ts. Ts must all be unique
    template<typename T, typename... Ts>
    struct type_pack_index;

    template<typename T, typename... Ts>
    struct type_pack_index<T, T, Ts...> : std::integral_constant<std::size_t, 0>
    {};

    template<typename T, typename U, typename... Ts>
    struct type_pack_index<T, U, Ts...> : std::integral_constant<std::size_t, 1 + type_pack_index<T, Ts...>::value>
    {};

    std::tuple<typename Keys::value_t...> value_list;

public:
    explicit constexpr type_map(const Keys::value_t... values) :
        value_list{ values... }
    {}

    type_map(type_map&&)                         = default;
    type_map(const type_map&)                    = default;
    auto operator=(type_map&&) -> type_map&      = default;
    auto operator=(const type_map&) -> type_map& = default;
    ~type_map()                                  = default;

    template<typename Key>
    [[nodiscard]] constexpr auto get() & -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }

    template<typename Key>
    [[nodiscard]] constexpr auto get() && -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }

    template<typename Key>
    [[nodiscard]] constexpr auto get() const& -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }

    template<typename Key>
    [[nodiscard]] constexpr auto get() const&& -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto get() & -> auto&&
    {
        return std::get<Index>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto get() && -> auto&&
    {
        return std::get<Index>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto get() const& -> auto&&
    {
        return std::get<Index>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto get() const&& -> auto&&
    {
        return std::get<Index>(value_list);
    }
};

}  // namespace tsri::utility::types

/* Structured binding support for the type_map class. */
namespace std
{

template<typename... Keys>
struct tuple_size<tsri::utility::types::type_map<Keys...>> : public integral_constant<std::size_t, sizeof...(Keys)>
{};

template<std::size_t N, typename... Keys>
struct tuple_element<N, tsri::utility::types::type_map<Keys...>>
{
    using type = Keys...[N] ::value_t;
};

}  // namespace std
