#!/usr/bin/env python3
"""Program a connected Teensy 4.1 via USB CDC baud=134 and a PJRC CLI loader."""
import argparse
import os
from pathlib import Path
import subprocess
import sys
import termios


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', required=True)
    parser.add_argument('--loader', required=True)
    parser.add_argument('--hex', required=True)
    args = parser.parse_args()
    if sys.platform != 'darwin':
        parser.error('This procedure is specifically for macOS')
    port = Path(args.port)
    loader = Path(args.loader).expanduser().resolve()
    firmware = Path(args.hex).expanduser().resolve()
    if not port.exists() or not str(port).startswith('/dev/cu.usbmodem'):
        parser.error('Expected an existing /dev/cu.usbmodem... port')
    if not loader.is_file() or not os.access(str(loader), os.X_OK) or not firmware.is_file():
        parser.error('Loader executable and HEX file must exist')
    process = subprocess.Popen([str(loader), '--mcu=TEENSY41', '-w', '-v', str(firmware)])
    try:
        fd = os.open(str(port), os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
        try:
            attrs = termios.tcgetattr(fd)
            attrs[4] = attrs[5] = termios.B134
            termios.tcsetattr(fd, termios.TCSANOW, attrs)
        finally:
            os.close(fd)
        return process.wait(timeout=25)
    except subprocess.TimeoutExpired:
        print('Teensy did not finish programming within 25 seconds', file=sys.stderr)
        return 2
    finally:
        if process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()


if __name__ == '__main__':
    sys.exit(main())
