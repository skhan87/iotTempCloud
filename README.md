# Firmware for iot temperature project with aconno Devkit

This is the firmware repository for an aconno Devkit  

It is the base for every new project.
Therefor it is the most up to date repository, since most submodules are developed in here.

## Features

## Working with aconno firmware

To get started with aconno firmware, here is a collection of links:
*  general documentation of aconno firmware, coding standard e.g.: https://git.simvelop.de/aconnoProducts/Documentation
*  developer section of the aconno website: https://aconno.de/developers/
*  guide for setting up the toolchain: https://aconno.de/toolchain/
*  git reopsitory with aconno products: https://git.simvelop.de/aconnoProducts

## Cloning

To clone this repo install git and type:
```cmd
git clone --recurse-submodules -b develop https://github.com/skhan87/iotTempCloud.git
```

This will automatically download all the repositories you need.
Now you can call make in the root folder of the project:

```cmd
make nrf52840_debug upload
```

Will compile and upload the code with debug settings, if there is any programmer connected.
Happy coding.
