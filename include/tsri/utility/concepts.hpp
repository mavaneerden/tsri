#pragma once

#include <concepts>
#include <type_traits>

namespace tsri::utility::concepts
{

/**
 * Used in are_types_unique concept.
 *
 * Recursively checks if all types are unique, for every type.
 * This is O(n²) so quite expensive, but good enough.
 */
template<typename T, typename... Ts>
static constexpr bool are_types_unique_v = (!std::is_same_v<T, Ts> and ...) and are_types_unique_v<Ts...>;

/**
 * Used in are_types_unique concept.
 *
 * In case there is only one type.
 */
template<typename T>
static constexpr bool are_types_unique_v<T> = true;

/**
 * @brief Checks if there are no duplicate types in the given variadic template.
 *
 * Source: https://stackoverflow.com/a/76043402
 *
 * @tparam Ts Types to check.
 */
template<class... Ts>
concept are_types_unqiue = are_types_unique_v<Ts...>;

/**
 * @brief Checks if a type is contained inside a type list.
 *
 * @tparam T  Type to check.
 * @tparam Ts Type list.
 */
template<typename T, typename... Ts>
static constexpr bool is_type_in_list_v = (std::is_same_v<T, Ts> or ...);

template<typename T, typename... Ts>
concept is_type_in_list = is_type_in_list_v<T, Ts...>;

template<auto V, auto... Vs>
static constexpr bool are_values_unique = ((V != Vs) and ...) and are_values_unique<Vs...>;

template<auto V>
static constexpr bool are_values_unique<V> = true;

template<typename T>
static constexpr bool is_unsigned_integral_v = std::unsigned_integral<T>;

}  // namespace tsri::utility::concepts
