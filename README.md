# Type-Safe Register Interface (TSRI)

This repository contains a zero-overhead header-only type-safe peripheral register interface.
It can be used to access the hardware registers in a baremetal environment.

## Usage
TODO

The user provides an SVD file location, namespace and output directory; TSRI then automaticallly generates the required register code according to the SVD file.

## Supported devices
Currently, only the RP2040 processor is supported.

## Disclaimer
No AI was used in the making of TSRI. All code is hand-written by me, except when explicitly stated otherwise.
In such a case, a source from where I took the code is provided.
