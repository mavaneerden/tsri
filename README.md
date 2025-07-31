# Type-Safe Register Interface (TSRI)

This repository contains a zero-overhead* header-only type-safe peripheral register interface.
It can be used to access the hardware registers in a baremetal environment.


*when using optimization level bigger than -O0

## Usage

### Integration into your codebase
The user provides an SVD file location, namespace and output directory; TSRI then automaticallly generates the required register code according to the SVD file.

### Examples
```cpp
// Assume register `reg` has fields `field1`, `field2`, etc.

// Get full register contents
const auto reg_value = reg::get();

// Get the values of each given field
const auto fields = reg::get_fields<reg::field1, reg::field2, ...>();
const auto field1_value = fields.get<reg::field1>();
const auto field2_value = fields.get<reg::field2>();
...

// If -DTSRI_OPTION_ENABLE_STRUCTURED_BINDING_GET_FIELDS flag is set:
const auto [field1_value, field2_value, ...] = reg::get_fields<reg::field1, reg::field2, ...>();

// Check if any of the given bits are set
const bool result = reg::is_any_bit_set(
    reg::field1 {
        reg::field1::bit::BIT0,
        reg::field1::bit{ 3U }
    },
    reg::field2 {
        reg::field2::bit::BIT1
    },
    ...
);

// Check if all of the given bits are set
const bool result = reg::are_all_bits_set( ... ); // same format as is_any_bit_set

// Set the given fields to the given values
reg::set_fields(
    reg::field1::value::SOME_VALUE,
    reg::field2::value{ 4U },
    reg::field3::reset_value,
    ...
);

// Set the given fields to the given values, overwriting the
// unspecified register fields with their reset value.
reg::set_fields_overwrite( ... ); // same format as set_fields

```


## Supported devices
Currently, only the RP2040 processor is supported.

## Disclaimer
No AI was used in the making of TSRI. All code is hand-written by me, except when explicitly stated otherwise.
In such a case, a source from where I took the code is provided.
