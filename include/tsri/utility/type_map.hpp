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

    // TODO: perfect forwarding???
    template<typename Key>
    constexpr auto get() const -> const Key::value_t&
    {
        return std::get<type_pack_index<Key, Keys...>::value>(value_list);
    }
};

}  // namespace tsri::utility::types

// TODO: structured binding support???
// namespace std
// {
//     template<typename... Keys>
//     struct tuple_size<picobaremetal::utility::type_map<Keys...>> : integral_constant<size_t, sizeof...(Keys)> {};

//     template<std::size_t Index, typename... Keys>
//     struct tuple_element<Index, picobaremetal::utility::type_map<Keys...>> : std::tuple_element<Index,
//     std::tuple<Keys...>> {};
// }
