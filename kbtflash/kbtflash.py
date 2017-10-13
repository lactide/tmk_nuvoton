#! /usr/bin/env python3

import argparse
import time
import sys
import usb.core
import usb.util
import operator
import struct
from functools import reduce

class BaseKeyboard:

    def __init__(self, idVendor, idProduct):
        self.dev = usb.core.find(idVendor = idVendor, idProduct = idProduct)
        if self.dev is None:
            raise RuntimeError("Device not found")
        self.unbind_kernel_drivers()

    def unbind_kernel_drivers(self):
        for itf in [0, 1]:
            if self.dev.is_kernel_driver_active(itf):
                self.dev.detach_kernel_driver(itf)

    def close(self):
        usb.util.dispose_resources(kb.dev)

class ISPKeyboard(BaseKeyboard):

    def get_report(self):
        bmRequestType = usb.util.build_request_type(
                            usb.util.CTRL_IN,
                            usb.util.CTRL_TYPE_CLASS,
                            usb.util.CTRL_RECIPIENT_INTERFACE)
        return self.dev.ctrl_transfer(bmRequestType,
                                      0x01,
                                      0x0301,
                                      0,
                                      9)

    def set_report(self, buf):
        assert len(buf) == 8
        bmRequestType = usb.util.build_request_type(
                            usb.util.CTRL_OUT,
                            usb.util.CTRL_TYPE_CLASS,
                            usb.util.CTRL_RECIPIENT_INTERFACE)
        self.dev.ctrl_transfer(bmRequestType,
                               0x09,
                               0x0301,
                               0,
                               buf)

    def send_packet(self, data):
        data = bytes([0x01, 0xea, len(data)]) + data
        if len(data) < 8:
            data += b"\x00" * (8 - len(data))
        if len(data) == 15:
            data += bytes([reduce(operator.xor, data)])
        from binascii import hexlify
        print(hexlify(data))
        for x in range(0, len(data), 8):
            self.set_report(data[x:x + 8])

        data = self.get_report()
        assert data[0] == 0xed
        ln = data[1]
        ret = data[2:].tobytes()
        while len(ret) < ln:
            ret += self.get_report().tobytes()
        return ret[:ln]

    def send_write_packet(self, addr, data):
        assert len(data) == 8
        self.send_packet(bytes([0xa1]) +
                         struct.pack(">h", addr) +
                         data[0:2] +
                         bytes([0x01]) +
                         data[2:8])

    def to_normal(self):
        self.send_packet(bytes([0xaf, 0x47]))

    def flash(self, fwIdProduct, data):
        self.send_packet(bytes([0xa5, 0x8b, 0x9b, 0x4b, 0x9c,
                                0x01, (fwIdProduct & 0xff) ^ 0xfd, 0xeb, 0xff, 0xff, 0xff, 0xff]))
        self.send_packet(bytes([0xa4, 0x4c]))
        # This is the order the official flashing tool does it, no idea why
        for x in range(256, len(data), 8):
            self.send_write_packet(x, data[x:x + 8])
        for x in range(0, 256, 8):
            self.send_write_packet(x, data[x:x + 8])
        self.send_packet(bytes([0xa8, 0x40]))
        self.to_normal()


class Keyboard(BaseKeyboard):

    def __init__(self, idVendor, idProduct):
        super(Keyboard, self).__init__(idVendor, idProduct)
        self._version = None

    def command(self, cmd, read = 0):
        self.dev.ctrl_transfer(0x21, 9, wValue = 0x0301, wIndex = 0x0001,
                          data_or_wLength = cmd)
        if read:
            ret = b""
            for _ in range(read):
                ret += bytes(self.dev.ctrl_transfer(0xa1, 1, wValue = 0x0301,
                                                    wIndex = 0x0001,
                                                    data_or_wLength = 9))
            return ret

    @property
    def version(self):
        if self._version is None:
            ret = self.command(b"\x01\xea\x02\xb0\x58\x00\x00\x00", read = 3)
            self._version = ret[4:12].decode()
        return self._version

    def to_isp(self):
        self.command(b"\x01\xea\x02\xa0\x48\x00\x00\x00")
        idVendor = self.dev.idVendor
        for _ in range(10):
            time.sleep(0.3)
            try:
                return ISPKeyboard(idVendor, 0x0000)
            except RuntimeError as e:
                pass
        raise e

def obfuscate(inp):
    # Note that this works for deobfuscation too
    return bytes((((b & 0x0f) << 4) | ((b & 0xf0) >> 4)) ^ 0xff for b in inp)

def auto_int(x):
    return int(x, 0)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog = "kbtflash"
    )

    parser.add_argument(
        "-v",
        "--verbose",
        action = "store_true"
    )

    subparsers = parser.add_subparsers(
        dest = "operation"
    )

    parser_version = subparsers.add_parser(
        "version",
        help = "Show FW version"
    )

    parser_isp = subparsers.add_parser(
        "isp",
        help = "Switch to ISP"
    )

    parser_flash = subparsers.add_parser(
        "flash",
        help = "Flash a file"
    )
    parser_flash.add_argument("file")
    parser_flash.add_argument(
        "-r",
        "--raw",
        help = "Flash as-is, without obfuscating first",
        action = "store_true"
    )
    parser_flash.add_argument(
        "-f",
        "--force",
        help = "Do not check whether the image starts with a valid stack pointer",
        action = "store_true"
    )

    for par in [parser_isp, parser_version, parser_flash]:
        par.add_argument(
            "-i",
            "--vendor-id",
            default = 0x0f39,
            type = auto_int
        )

        par.add_argument(
            "-p",
            "--product-id",
            default = 0x0671,
            type = auto_int
        )

    parser_obf = subparsers.add_parser(
        "obf",
        help = "Obfuscate a file"
    )

    parser_deobf = subparsers.add_parser(
        "deobf",
        help = "Deobfuscate a file"
    )

    for par in [parser_obf, parser_deobf]:
        par.add_argument("infile")
        par.add_argument("outfile")

    args = parser.parse_args()

    if args.operation in ["version", "isp"]:
        kb = Keyboard(args.vendor_id, args.product_id)
        if args.operation == "version":
            if args.verbose:
                print("Version: %s" % kb.version)
            else:
                print("%s" % kb.version)
        elif args.operation == "isp":
            kb.to_isp()
        kb.close() # Would get released automatically, but this makes it easier to debug in ipython
    elif args.operation == "flash":
        with open(args.file, "rb") as f:
            data = f.read()
        if not args.raw:
            data = obfuscate(data)
        if not args.force:
            addr, = struct.unpack("<I", obfuscate(data[:4]))
            if not (0x20000000 < addr < 0x2000ffff):
                print("This does not seem right - the initial stack pointer points to a weird address %08x" % addr)
                sys.exit()
        kb = ISPKeyboard(args.vendor_id, 0x0000)
        print("Flashing...")
        kb.flash(args.product_id, data) # And now pray
        print("Done")
        kb.close()

    elif args.operation in ["obf", "deobf"]:
        with open(args.infile, "rb") as fin, open(args.outfile, "wb") as fout:
            fout.write(obfuscate(fin.read()))
    else:
        parser.print_help()
