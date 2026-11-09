import argparse
import hashlib
import hmac
import json
import math
import re
import secrets
import sys
import time
from pathlib import Path


def serial_module():
    try:
        import serial
    except ImportError:
        raise ValueError("Installer pyserial : python3 -m pip install pyserial")
    return serial


UINT32MAX = 2**32 - 1
MAX_FRAME = 160


def unsigned(value):
    number = int(value)
    if not 0 <= number <= UINT32MAX:
        raise ValueError(f"Value must be between 0 and {UINT32MAX}, got {value}")
    return number


def payload(device, seq, command, value) -> bytes:
    if not re.fullmatch(r"[A-Za-z0-9_-]{1,32}", device):
        raise ValueError(f"Invalid device id: {device}.")

    seq, value = unsigned(seq), unsigned(value)
    if command not in ("LED", "GET"):
        raise ValueError("Command must be LED or GET.")
    if command == "LED" and not 1 <= value <= 60000:
        raise ValueError("Led duration must be between 1 and 60000.")
    if command == "GET" and value != 0:
        raise ValueError("Get don't ask for value.")
    return f"{device}|{seq}|{command}|{value}".encode("ascii")


def build_frame(device, seq, command, value, key) -> bytes:
    body = payload(device, seq, command, value)
    tag = hmac.new(key, body, hashlib.sha256).hexdigest().encode("ascii")
    return body + b"|" + tag + b"\n"


def parse_frame(wire):
    if len(wire) > MAX_FRAME or not wire.endswith(b"\n"):
        raise ValueError(f"Len of wire exceed {MAX_FRAME} or wire don't ends with LF")
    parts = wire[:-1].decode("ascii").split("|")  # delete "\n"
    if len(parts) != 5:
        raise ValueError("Invalide Wire format")

    device, seq, command, value, tag = parts
    if not re.fullmatch(r"[0-9a-f]{64}, tag"):
        raise ValueError(
            f"Unexpected HMAC, waiting for 64 lower chars, got {len(hmac)}"
        )
    return [device, seq, command, value, tag]


def save_capture(path, wire):
    Path(path).write_text(
        json.dumps(
            {
                "format": "uart-tool",
                "wire_hex": wire.hex(),
                "preview": wire.decode("ascii"),
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )


def load_capture(path):
    data = json.loads(Path(path).read_text(encoding="utf-8"))
    if not isinstance(data, dict) or data.get("format") != "uart-tp-v1":
        raise ValueError("Format de capture inconnu")
    encoded = data.get("wire_hex")
    if not isinstance(encoded, str) or len(encoded) > MAX_FRAME * 2:
        raise ValueError("Capture invalide ou trop longue")
    wire = bytes.fromhex(encoded)
    parse_frame(wire)
    return wire


def receive(port, duration):
    deadline = None if duration is None else time.monotonic() + duration
    while deadline is None or time.monotonic() < deadline:
        chunk = port.read(min(max(port.in_waiting, 1), 4096))
        if chunk:
            sys.stdout.write(chunk.decode("utf-8", errors="replace"))
            sys.stdout.flush()


def exchange(args, wire=None):
    if args.dry_run:
        if wire is None:
            raise ValueError("--dry-run don't works on this monitor")
        print("TX (sim) :", wire.decode("ascii"), end="")
        return

    if not args.port:
        raise ValueError("Provide --port, e.g. --port /dev/ttyUSB0")

    serial = serial_module()
    with serial.Serial(
        None,
        args.baud,
        timeout=0.1,
        write_timeout=2,
        xonxoff=False,
        rtscts=False,
        dsrdtr=False,
    ) as port:
        port.dtr = False
        port.rts = False
        port.port = args.port
        port.open()
        receive(port, args.startup_wait)
        if wire is None:
            print("Listening to UART, CTRL+C to close.")
            receive(port, None)
        else:
            print("TX: ", wire.decode("ascii"), end="", flush=True)
            if port.write(wire) != len(wire):
                raise OSError("Failed to write on UART")
            receive(port, args.listen)


def parser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--port", help="UART port: e.g. /dev/ttyUSB0")
    p.add_argument("--baud", type=int, default=115200)
    p.add_argument("--listen", type=float, default=2, help="Listen seconds after send.")
    p.add_argument(
        "--startup-wait",
        type=float,
        default=0.5,
        help="Listen after start before sending.",
    )
    p.add_argument("--dry-run", action="store_true", help="Without opening UART.")

    sub = p.add_subparsers(dest="action", required=True)
    sub.add_parser("ports", help="List all available ports.")
    sub.add_parser("monitor", help="Shows received data.")
    keygen = sub.add_parser("keygen", help="Create random key.")
    keygen.add_argument("path", help="Output file.")
    send = sub.add_parser("send", help="Build and send authentified command.")
    send.add_argument("--key-file", required=True)
    send.add_argument("--device", default="board01")
    send.add_argument("--sequence", required=True, type=int)
    send.add_argument("--command", choices=["led", "get"], default="led")
    send.add_argument("--duration", type=int, default=1000, help="LED duration ms.")
    send.add_argument("--capture", help="Save data before send.")
    replay = sub.add_parser("replay", help="Resent capture without key.")
    replay.add_argument("capture")
    tamper = sub.add_parser("tamper", help="Change a field without changing the HMAC.")
    tamper.add_argument("capture")
    tamper.add_argument(
        "--field", required=True, choices=["device", "sequence", "command", "duration"]
    )
    tamper.add_argument("--value", required=True)
    tamper.add_argument("--output", required=True)
    tamper.add_argument(
        "--transmit", action="store_true", help="Send the altered wire."
    )
    return p


def main(argv=None):
    args = parser().parse_args(argv)
    for name in ("listen", "startup_wait"):
        value = getattr(args, name)
        if not math.isfinite(value) or value < 0:
            raise ValueError("Time value must be finite.")
    if args.baud <= 0:
        raise ValueError("Invalid BAUDRATE.")

    if args.action == "keygen":
        with Path(args.path).open("x", encoding="ascii") as f:
            f.write(secrets.token_hex(32) + "\n")
        print("Key created at :", args.path)
    elif args.action == "ports":
        serial_module()
        from serial.tools import list_ports

        for port in list_ports.comports():
            print(f"{port.device}\t{port.description}")
    elif args.action == "monitor":
        exchange(args)
    elif args.action == "send":
        key = bytes.fromhex(Path(args.key_file).read_text(encoding="ascii").strip())
        if len(key) != 32:
            raise ValueError(f"Key must be 32 bytes, got {len(key)}")
        value = args.duration if args.command == "led" else 0
        wire = build_frame(args.device, args.sequence, args.command.upper(), value, key)
        if args.capture:
            save_capture(args.capture, wire)
        exchange(args, wire)
    elif args.action == "replay":
        exchange(args, load_capture(args.capture))
    elif args.action == "tamper":
        fields = parse_frame(load_capture(args.capture))
        index = {"device": 0, "sequence": 1, "command": 2, "duration": 3}[args.field]
        fields[index] = args.value.upper() if args.field == "command" else args.value
        body = payload(*fields[:4])
        wire = body + b"|" + fields[4].encode("ascii") + b"\n"
        save_capture(args.output, wire)
        print("Altered wire saved, HMAC conserved: ", args.output)
        if args.transmit:
            exchange(args, wire)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.", file=sys.stderr)
    except Exception as exc:
        print(f"Error : {exc}", file=sys.stderr)
        sys.exit(1)
