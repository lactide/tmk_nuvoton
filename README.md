TMK/ChibiOS for PokerII
=======================
(or other boards with the Nuvoton NUC122)

This is an unofficial keyboard firmware for the Vortex Poker II with the
Nuvoton NUC122SC1AN MCU. They aren't produced anymore, the current version
of the PokerII seems to use another MCU. As far as i know you can identify
the older Poker II by simply looking at the usb port.
If it has a mini usb port, then it is a Poker II with the NUC122 MCU.

**This firmware comes without support.**
**You can't fix your bricked keyboard,**
**if you can't solder SWD headers to your board.**

Keymap
------
By default the keymap "poker2" is used, which is available at src/keymaps/poker2/keymap_poker2.c.
Please read the sourcecode of it.
You can change the keymap by creating a new keymap in the same directory with the same naming
scheme and call make with "KEYMAP=yourkeymapname".


Development
-----------
If you like to help with the development of this TMK port, you should solder a header to the 5 SWD pins
which are labelled GND, RST, ICE_CK, ICE_DAT1 and VCC and use an ARM debugger like the J-Link.

The bootloader of the PokerII enables read-protection for the flash, so you'll need to mass-erase
the NUC122 first. Afterwards you can flash and debug this firmware in the APROM.
If you like to use the firmware update binary in the future, you can reflash the original bootloader from
the bootloader/ directory to the LDROM. There is also a patched debug version of the bootloader, which won't
protect the flash from reading.

Moreover there is a very poor debugging mechanism if you stumble into a hardfault. The hardfault handler
will save some register variables and then do a reset. When the firmware starts again, it'll detect
that an hardfault happened and light up the right led of the spacebar. Then, by pressing Capslock+Tab,
the program counter which led to the hardfault will be typed by the firmware (at least if you keep
that feature in your keymap). If you saved the memory map of your compiled firmware, you can then pinpoint
the function which led to the hardfault.


Prerequisites
-------------
- ARM Toolchain
- srecord 1.64
- python3
- pyusb


Building
--------
```
$ git clone https://github.com/lactide/tmk_nuvoton.git
$ cd tmk_nuvoton
$ git submodule update --init --recursive --remote
$ make
```

Flashing
--------
Create a flashable binary:  
```$ make build/ikbc_poker2.bin```

This binary can be flashed by the original firmware updater or by the kbtflash.py script in this repository.
Please note that kbtflash.py might need the --product-id and --vendor-id flags.

This tmk port currently doesn't understand the "go to bootloader" usb command. You'll have to provide a way
to enter the bootloader via tmk. The default keymap_poker2.c will enter the bootloader by pressing CapsLock+Esc.

You might flash your board directly after entering the bootloader:  
```$ make flash```


Missing Features
================
- support for the AT24C02C eeprom
- support for the Data Flash section of the NUC122
- support for the backlit version of poker2
- support other keyboards with a NUC122
