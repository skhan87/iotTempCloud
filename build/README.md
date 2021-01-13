# aconno build library

Basic aconno library, found in all aconno projects. Contains scripts and tools used to build
and maintain aconno projects, as well as generate OTA updates.


# Over-The-Air functionality

OTA update functionality is available in all aconno projects which enabled it by setting 
main project Makefile variable 'ENABLE_OTA_UPDATES := YES'.

Build library provides two scripts used for generation of OTA encryption keys and packing
OTA updates. Scripts work out-of-the-box, are easy to use and well documented.
They can be found in /build/tools directory.
