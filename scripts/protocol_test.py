#!/usr/bin/env python3
"""B1024-01 测试治具协议帧校验与解析对齐测试（与 ResponseParser / AppleScript 一致）。"""

from __future__ import annotations

from dataclasses import dataclass
from typing import List, Optional, Tuple


def modbus_crc16(data: bytes, init: int = 0xFFFF) -> int:
    crc = init
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return crc


def append_crc(body: bytes) -> bytes:
    crc = modbus_crc16(body)
    return body + bytes([crc & 0xFF, (crc >> 8) & 0xFF])


def verify_frame(hex_str: str) -> bool:
    raw = bytes.fromhex(hex_str.replace(" ", ""))
    if len(raw) < 5:
        return False
    length = raw[2]
    if len(raw) != length:
        return False
    body = raw[: length - 2]
    expected = raw[length - 2] | (raw[length - 1] << 8)
    return modbus_crc16(body) == expected


@dataclass
class VoltageReading:
    fault: int
    voltage_v: float

    @property
    def ok(self) -> bool:
        return self.fault == 0x00


def parse_voltage(info: bytes, offset: int = 0) -> Optional[VoltageReading]:
    if len(info) < offset + 3:
        return None
    fault = info[offset]
    value = (info[offset + 1] << 8) | info[offset + 2]
    return VoltageReading(fault=fault, voltage_v=value / 100.0)


def parse_response(hex_str: str) -> Tuple[int, int, str]:
    raw = bytes.fromhex(hex_str.replace(" ", ""))
    cmd = raw[4]
    resp = raw[5]
    info = raw[6:-2]

    if resp != 0x00:
        return cmd, resp, f"RESP=0x{resp:02X}"

    if cmd == 0x01:
        return cmd, resp, info.decode("ascii", errors="replace")

    if cmd == 0x02 and len(info) >= 9:
        parts = []
        for name, off in [("VCC_12V", 0), ("VCC_5V", 3), ("VCC_3.3V", 6)]:
            r = parse_voltage(info, off)
            parts.append(f"{name}: {r.voltage_v:.2f}V ({'OK' if r.ok else 'Fault'})")
        return cmd, resp, "; ".join(parts)

    if cmd in (0x03, 0x04, 0x05, 0x06) and len(info) >= 3:
        r = parse_voltage(info)
        labels = {0x03: "CN43", 0x04: "CN39", 0x05: "CN47", 0x06: "CN13"}
        return cmd, resp, f"{labels[cmd]}: {r.voltage_v:.2f}V ({'OK' if r.ok else 'Fault'})"

    if cmd == 0x07 and len(info) >= 1:
        return cmd, resp, f"ST_INPUT fault={'OK' if info[0] == 0 else 'Fault'}"

    return cmd, resp, info.hex(" ")


# 协议文档 3.1–3.7 示例帧（request, response）
PROTOCOL_CASES = [
    ("3.1 QueryVersion", "02 7F 08 01 01 FF 06 43", "02 7F 15 01 01 00 56 31 2E 30 2E 30 2E 32 36 30 35 33 30 5E DD"),
    ("3.2 VCC CN52", "02 7F 08 01 02 FF 06 B3", "02 7F 11 01 02 00 00 04 BA 00 01 F1 00 01 49 A3 09"),
    ("3.3 Printer CN43", "02 7F 08 01 03 FF 07 23", "02 7F 0B 01 03 00 00 04 B4 A9 52"),
    ("3.4 5V CN39", "02 7F 08 01 04 FF 05 13", "02 7F 0B 01 04 00 00 01 F0 1F F1"),
    ("3.5 12V CN47", "02 7F 08 01 05 FF 04 83", "02 7F 0B 01 05 00 00 04 B5 E0 92"),
    ("3.6 Proximity CN13", "02 7F 08 01 06 FF 04 73", "02 7F 0B 01 06 00 00 01 F0 66 31"),
    ("3.7 ST_INPUT IO", "02 7F 09 01 07 FF 00 1E C3", "02 7F 09 01 07 00 00 5F 33"),
]


def run_tests() -> int:
    failed = 0
    print("=== CRC verification ===")
    for name, req, rsp in PROTOCOL_CASES:
        ok_req = verify_frame(req)
        ok_rsp = verify_frame(rsp)
        status = "PASS" if ok_req and ok_rsp else "FAIL"
        print(f"[{status}] {name}")
        if not ok_req:
            print(f"  request CRC/len invalid: {req}")
            failed += 1
        if not ok_rsp:
            print(f"  response CRC/len invalid: {rsp}")
            failed += 1

    print("\n=== Response parsing ===")
    for name, _req, rsp in PROTOCOL_CASES:
        cmd, resp, text = parse_response(rsp)
        print(f"[{name}] CMD=0x{cmd:02X} -> {text}")

    print("\n=== AppleScript pattern keys (no spaces) ===")
    for name, req, rsp in PROTOCOL_CASES:
        key = req.replace(" ", "")
        consume = len(bytes.fromhex(key))
        print(f"{name}: pattern={key} consume={consume} response={rsp}")

    return failed


if __name__ == "__main__":
    raise SystemExit(run_tests())
