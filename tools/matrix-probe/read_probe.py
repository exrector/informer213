import serial, sys, time, glob
log = open(sys.argv[1], "a", buffering=1)
while True:
    try:
        port = (glob.glob("/dev/cu.usbmodem*") or [None])[0]
        if not port: time.sleep(0.3); continue
        ser = serial.Serial(port, 115200, timeout=0.2)
        while True:
            line = ser.readline().decode(errors="replace").strip()
            if line: log.write(f"{time.strftime('%H:%M:%S')} {line}\n")
    except Exception as e:
        log.write(f"{time.strftime('%H:%M:%S')} LOGGER-RECONNECT {e}\n"); time.sleep(0.5)
