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
inline constexpr bool are_types_unique_v = (!std::is_same_v<T, Ts> and ...) and are_types_unique_v<Ts...>;

template<typename T>
inline constexpr bool are_types_unique_v<T> = true;

/**
 * @brief Checks if there are no duplicate types in the given variadic template.
 *
 * Source: https://stackoverflow.com/a/76043402
 */
template<class... Ts>
concept are_types_unqiue = are_types_unique_v<Ts...>;


template<template<typename A, typename B> typename SimilarityCondition, typename T, typename... Ts>
inline constexpr bool is_similar_type_in_list_v = (SimilarityCondition<T, Ts>::value or ...);

template<typename T, typename... Ts>
inline constexpr bool is_type_in_list_v = is_similar_type_in_list_v<std::is_same, T, Ts...>;

template<typename T, typename... Ts>
concept is_type_in_list = is_type_in_list_v<T, Ts...>;

template<template<typename A, typename B> typename SimilarityCondition, typename T, typename U, typename... Ts>
inline constexpr bool are_similar_types_grouped_in_list =
    SimilarityCondition<T, U>::value ? are_similar_types_grouped_in_list<SimilarityCondition, T, Ts...>
                                     : not is_similar_type_in_list_v<SimilarityCondition, T, Ts...> and
                                           are_similar_types_grouped_in_list<SimilarityCondition, U, Ts...>;

template<template<typename A, typename B> typename SimilarityCondition, typename T, typename U>
inline constexpr bool are_similar_types_grouped_in_list<SimilarityCondition, T, U> = true;

template<template<typename A, typename B> typename SimilarityCondition, typename T>
inline constexpr bool are_similar_types_grouped_in_list<SimilarityCondition, T, T> = true;


template<auto V, auto... Vs>
inline constexpr bool are_values_unique = ((V != Vs) and ...) and are_values_unique<Vs...>;

template<auto V>
inline constexpr bool are_values_unique<V> = true;

}  // namespace tsri::utility::concepts
