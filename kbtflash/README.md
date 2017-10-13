## kbtflash

Some time ago, atalax reversed the protocol used by KBT Pure Pro for flashing its firmware in
a hope of being able to flash a custom firmware (or do some BadUSB demonstration). This
script is able to upload a firmware binary onto the keyboard.

The binary is obfuscated (already solved on [SE](http://reverseengineering.stackexchange.com/questions/5945/finding-the-actual-thumb-code-in-firmware)) The firmware has a trailer and a 32-bit checksum.
If the uploaded firmware checksum does not match, the keyboard stays in the bootloader.

The MCU used is Nuvoton NUC122, which has a standard SWD interface.
It has multiple sections of flash: APROM for the application, LDROM for the bootloader and a section for Userdata. The flash-lock bit is set, it is only possible to dump the bootloader by uploading a valid firmware and sending the contents of the LDROM. 

There also is an SDK from Nuvoton containing some sort of a bootloader, but it
does not seem to have anything in common with the KBT provided one.

@atalax: Thanks!
