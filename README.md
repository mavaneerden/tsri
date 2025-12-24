# Type-Safer Register Interface (TSRI)

The goal of this repository is to experiment with a zero-runtime-overhead* header-only peripheral register interface that is more type-safe than common register abstractions, such as CMSIS's unions.

Some ways it achieves more type safety:
- Fields with enumerated values cannot be set to a different value.
- Fields that consist of one bit cannot be set to anything other than 1 or 0.
- Read-only fields cannot be written.
- Write-only fields cannot be read.
- Write-clear fields can only be set to 1.
- Self-clearing fields can only be set to 1.

In addition, there are some measures that make it harder for the user to create bugs:
- Field bits can only be set inside that field: avoids confusing different bit positions between different fields.
- Field values can only be set inside the register that the fields belong to: avoids confusing different fields between different registers.
- A field cannot be set more than once per operation: avoids unintended duplicate operations.
- Field values and bit positions are automatically shifted and bitmasked correctly: no manual fidgetting with shifts and bitmasks, which is error-prone.


*when using optimization level bigger than -Og and not defining TSRI_OPTION_DISABLE_ALWAYS_INLINE

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
        reg::field1::bit{ runtime_variable }
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

## Limitations
- Extremely slow compilation due to lots of metaprogramming makes this project impractical to use. A precompiled header
  is *required* to keep compilation times down. Generating the precompiled header takes about 5m40s on my machine for
  the rp2040 example. Subsequent compilations with a change in `blink.cpp` take about 4.5s. This is OK for this example,
  but one shudders to imagine what the compile times would be for projects with 100s of files.
- Constant values for fields that can have any value are not checked
- Runtime variables are not checked

## Disclaimer
No AI was used in the making of TSRI. All code is hand-written by me, except when explicitly stated otherwise.
In such a case, a source from where I took the code is provided.
