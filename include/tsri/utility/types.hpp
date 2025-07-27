#pragma once

#include <cstdint>

namespace tsri::utility::types
{

/**
 * @brief Type for register addresses.
 */
using register_address_t = uintptr_t;

/**
 * @brief Type for register values.
 */
using register_value_t = uint32_t;

/**
 * @brief Type for pointers to registers.
 */
using register_ptr_t = volatile register_value_t* const;

/**
 * @brief Size type for register, should at most fit 32 (since there are 32 bits).
 */
using register_size_t = uint8_t;

}  // namespace type_safe_register_interface::utility::types
