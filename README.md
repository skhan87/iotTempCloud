# Firmware for acnBEACON

This is the firmware repository for an aconno Beacon.

It is the base for every new project.
Therefor it is the most up to date repository, since most submodules are developed in here.

## Features

The acnBeacon itself has only a few features:
*  initial advertisement: Eddystone Advertisement with (ACONNO_ID(0x69), 0xFF, 0x00) as manufacturer data. Gets send 3 times when controller restarted
*  acnBeacon advertisement as described here: https://github.com/aconno/Sensorics/blob/master/docs/advertisement-deserialization.md, deserializers can be found here: https://aconno.de/sensorics/formats/
*  iBeacon advertisement: major: 0x1234, minor: 0x5678, UUID: [0x00, 0x11..0xFF]

## Working with aconno firmware

To get started with aconno firmware, here is a collection of links:
*  general documentation of aconno firmware, coding standard e.g.: https://git.simvelop.de/aconnoProducts/Documentation
*  developer section of the aconno website: https://aconno.de/developers/
*  guide for setting up the toolchain: https://aconno.de/toolchain/
*  git reopsitory with aconno products: https://git.simvelop.de/aconnoProducts

## Cloning

To clone this repo install git and type:
```cmd
git clone --recurse-submodules -b develop https://git.simvelop.de/aconnoProducts/acnBEACON/acnBEACONFW.git
```

This will automatically download all the repositories you need.
Now you can call make in the root folder of the project:

```cmd
make nrf52840_debug upload
```

Will compile and upload the code with debug settings, if there is any programmer connected.
Happy coding.