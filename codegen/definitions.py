from enum import Enum
from typing import List

class AccessType(Enum):
    READ_ONLY = "read-only"
    WRITE_ONLY = "write-only"
    READ_WRITE = "read-write"
    SELF_CLEARING = "self-clearing"
    WRITE_CLEAR = "write-clear"

    @staticmethod
    def is_register_write_only(access_type: 'AccessType') -> bool:
        return access_type in {AccessType.WRITE_ONLY, AccessType.SELF_CLEARING, AccessType.WRITE_CLEAR}

    @staticmethod
    def from_fields(fields: List['Field']) -> 'AccessType':
        access_types = set(field.access_type for field in fields)
        if AccessType.READ_WRITE in access_types:
            return AccessType.READ_WRITE
        if (AccessType.WRITE_ONLY in access_types or AccessType.SELF_CLEARING in access_types or AccessType.WRITE_CLEAR in access_types) and AccessType.READ_ONLY in access_types:
            return AccessType.READ_WRITE
        if AccessType.WRITE_ONLY in access_types or AccessType.SELF_CLEARING in access_types or AccessType.WRITE_CLEAR in access_types:
            return AccessType.WRITE_ONLY
        if AccessType.READ_ONLY in access_types:
            return AccessType.READ_ONLY
        raise ValueError("Invalid conversion from fields access to register access.")

class EnumValue:
    def __init__(self, name: str, description: str, value: str):
        self.name = name
        self.description = description
        self.value = EnumValue.parse_value(str(value))

    def __repr__(self):
        return f"{self.name} = {self.value}"

    @staticmethod
    def parse_value(value: str) -> int:
        if value.startswith("0x"):
            return int(value, 16)
        if value.startswith("0b") or value.startswith("#"):
            new_value = value.replace("x", "0") # The 'x' represents don't care, treat as zero
            return int(new_value, 2)
        return int(value)

class Field:
    def __init__(self, name: str, description: str, start_bit: int, length_in_bits: int, value_on_reset: int, access_type: AccessType, enum_values: List[EnumValue] = []):
        self.name = name
        self.description = description
        self.start_bit = start_bit
        self.length_in_bits = length_in_bits
        self.value_on_reset = value_on_reset
        self.access_type = access_type
        self.enum_values = enum_values

    def __repr__(self):
        enum_str =  "\n            " + "\n            ".join(str(enum) for enum in self.enum_values) if len(self.enum_values) > 0 else ""

        return f"{self.name} [{self.start_bit + self.length_in_bits - 1}:{self.start_bit}] = 0b{self.value_on_reset:0{self.bit_width}b} ({self.access_type.value}){enum_str}"

class Register:
    def __init__(self, name: str, description: str, base_address: int, address_offset: int, value_on_reset: int, supports_atomic_bit_operations: bool, access_type: AccessType, fields: List[Field] = []):
        self.name = name
        self.description = description
        self.base_address = base_address
        self.address_offset = address_offset
        self.value_on_reset = value_on_reset
        self.supports_atomic_bit_operations = supports_atomic_bit_operations
        self.access_type = access_type
        self.fields = fields

    def __repr__(self):
        field_str = "\n        ".join(str(field) for field in self.fields)

        return f"{self.name} @ 0x{self.base_address + self.address_offset:08X} = 0x{self.value_on_reset:08X} ({self.access_type.value}) {'ATOMIC' if self.supports_atomic_bit_operations else ''}\n        {field_str}"

class Peripheral:
    def __init__(self, name: str, description: str, base_address: int, registers: List[Register] = []):
        self.name = name
        self.description = description
        self.base_address = base_address
        self.registers = registers

    def __repr__(self):
        register_str = "\n    ".join(str(register) for register in self.registers)

        return f"{self.name} @ 0x{self.base_address:08X}\n    {register_str}"