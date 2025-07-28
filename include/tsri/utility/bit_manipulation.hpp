#pragma once

#include <concepts>

#include "types.hpp"

namespace tsri::utility::bit_manipulation {

    /**
     * @brief
     *
     * @param bit_positions
     * @return utility::types::register_value_t
     */
    static constexpr auto get_bit_positions_bitmask(const std::unsigned_integral auto... bit_positions)
        -> utility::types::register_value_t
    {
        return ((1U << bit_positions) | ...);
    }
}