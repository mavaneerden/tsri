#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "concepts.hpp"

namespace tsri::utility::types
{

namespace detail
{

template<typename... Keys>
    requires concepts::are_types_unqiue<Keys...>
class type_map_impl
{
private:
    template<typename Type, typename _, typename... TypePack>
    static constexpr std::size_t type_pack_index = 1 + type_pack_index<Type, TypePack...>;

    template<typename Type, typename TypePack>
    static constexpr std::size_t type_pack_index<Type, TypePack> = 0U;

    std::tuple<typename Keys::value...> value_list;

public:
    explicit constexpr type_map_impl(const Keys::value... values) :
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
    [[nodiscard]] constexpr auto get() const& noexcept -> auto&&
    {
        return std::get<Index>(value_list);
    }

    template<std::size_t Index>
    [[nodiscard]] constexpr auto get() const&& noexcept -> auto&&
    {
        return std::get<Index>(value_list);
    }
#endif

protected:
    template<typename Key>
        requires concepts::is_type_in_list<Key, Keys...>
    [[nodiscard]] constexpr auto get() const& noexcept -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>>(value_list);
    }

    template<typename Key>
        requires concepts::is_type_in_list<Key, Keys...>
    [[nodiscard]] constexpr auto get() const&& noexcept -> auto&&
    {
        return std::get<type_pack_index<Key, Keys...>>(value_list);
    }
};

}  // namespace detail

template<typename Key, typename... Keys>
class type_map : public detail::type_map_impl<Key, Keys...>
{
    using detail::type_map_impl<Key, Keys...>::type_map_impl;

public:
    template<typename KeyToGet>
    [[nodiscard]] constexpr auto get() const& noexcept -> auto&&
    {
        return detail::type_map_impl<Key, Keys...>::template get<KeyToGet>();
    }

    template<typename KeyToGet>
    [[nodiscard]] constexpr auto get() const&& noexcept -> auto&&
    {
        return detail::type_map_impl<Key, Keys...>::template get<KeyToGet>();
    }
};

template<typename Key>
class type_map<Key> : public detail::type_map_impl<Key>
{
    using detail::type_map_impl<Key>::type_map_impl;

public:
    [[nodiscard]] constexpr auto get() const& noexcept -> auto&&
    {
        return detail::type_map_impl<Key>::template get<Key>();
    }

    [[nodiscard]] constexpr auto get() const&& noexcept -> auto&&
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

template<std::size_t N, typename... Keys>
struct tuple_element<N, tsri::utility::types::detail::type_map_impl<Keys...>>
{
    using type = Keys...[N] ::value;
};

}  // namespace std
#endif
