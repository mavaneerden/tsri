import definitions as defs
from typing import List
from cmsis_svd.model import SVDField, SVDRegister, SVDDevice, SVDPeripheral

def get_valid_name(name):
    new_name = name

    if not name[0].isalpha():
        new_name = "_" + new_name

    return new_name

def get_enums_from_field(field: SVDField) -> List[defs.EnumValue]:
    enum_values = []
    for enumerated_value in field.enumerated_values or []:
        for enum_value in enumerated_value.enumerated_values:
            enum = defs.EnumValue(
                name=get_valid_name(enum_value.name),
                description=enum_value.description if enum_value.description is not None else "",
                value=enum_value.value
            )
            enum_values.append(enum)

    return enum_values

def get_fields_from_register(register: SVDRegister) -> List[defs.Field]:
    fields = []

    for field in register.fields:
        fld = defs.Field(
            name=field.name,
            description=field.description if field.description is not None else "",
            start_bit=field.bit_offset,
            length_in_bits=field.bit_width,
            value_on_reset=(register.reset_value & (((1 << field.bit_width) - 1) << field.bit_offset)) >> field.bit_offset,
            access_type=defs.AccessType.WRITE_CLEAR if field.modified_write_values is not None and field.modified_write_values.value == "oneToClear" else defs.AccessType(field.access.value),
            enum_values=get_enums_from_field(field)
        )

        if fld.enum_values == []:
            if fld.access_type == defs.AccessType.SELF_CLEARING or fld.access_type == defs.AccessType.WRITE_CLEAR:
                fld.enum_values.append(defs.EnumValue(name="ONE", description="", value="1"))
            elif field.bit_width == 1 and defs.AccessType != defs.AccessType.READ_ONLY:
                fld.enum_values.append(defs.EnumValue(name="ZERO", description="", value="0"))
                fld.enum_values.append(defs.EnumValue(name="ONE", description="", value="1"))

        fields.append(fld)
    return fields

def get_registers_from_peripheral(peripheral: SVDPeripheral) -> List[defs.Register]:
    registers = []
    for register in peripheral.registers:
        fields = get_fields_from_register(register)
        reg = defs.Register(
            name=register.name,
            description=register.description if register.description is not None else "",
            base_address=peripheral.base_address,
            address_offset=register.address_offset,
            value_on_reset=register.reset_value,
            supports_atomic_bit_operations= peripheral.name != "SIO",
            access_type=defs.AccessType.from_fields(fields),
            fields=fields
        )
        registers.append(reg)
    return registers

def parse_peripheral(peripheral: SVDPeripheral):
    return defs.Peripheral(
        name=peripheral.name,
        description=peripheral.description if peripheral.description is not None else "",
        base_address=peripheral.base_address,
        registers=get_registers_from_peripheral(peripheral)
    )

def parse_peripherals(device: SVDDevice) -> List[defs.Peripheral]:
    peripherals = []
    for peripheral in device.peripherals:
        periph = defs.Peripheral(
            name=peripheral.name,
            description=peripheral.description if peripheral.description is not None else "",
            base_address=peripheral.base_address,
            registers=get_registers_from_peripheral(peripheral)
        )
        peripherals.append(periph)
    return peripherals
