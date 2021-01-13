# NordicAL

Abstraction Layer for Nordic hardware and software development kit.

Contains parts of former aconnoNordicPAL and other libraries that where based on the Nordic SDK.


# BLE

Please refer to the BLE module Readme for further information.

# Flash

Flash is just the management class.
It contains general flash functions like Init and PrintStats.

Please set FDS_CRC_CHECK_ON_READ to 1 and FDS_CRC_CHECK_ON_WRITE to 1.
Otherwise about 1 / 10.000 entries will be corrupted. Enabling this
options lowers the number by at least a factor of ten.

At the same time, in this implementation there are some private functions,
that are internally used by befriended class inside the flash class.
This is to prevent their usage from the outside while still preserving
consistent usage of the underlying SDK library.

## Record

A string identified value in flash memory.
Instantiating this class does not read or write anything.
Only when you using its functions, flash memory
is accessed.

## Additions

If a new subclass under Flash is to be added,
befriend the Flash::try... member functions.
This will also bother with allocating buffers for
the underlying write and update commands.

## Test

To run the test, just add $(THIS_PATH)/test/FlashTest.cpp to include in Makefile.sub.
Your main function should look something like this:
```cpp
/**@brief Function for application main entry.
 */
int main(void)
{
    /* Enable the SoftDevice. */
    auto rc = nrf_sdh_enable_request();
    APP_ERROR_CHECK(rc);
    nrf_sdh_freertos_init(NULL, NULL);

    RTOS::Init();
}
```

## Internals

*  Records use the FileId = 0 all the time
*  Records calculate a recordKey by using CRC16 on their name