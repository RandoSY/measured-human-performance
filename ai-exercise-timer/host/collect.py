#!/usr/bin/env python3
import argparse, asyncio, csv, math
from datetime import datetime, timezone
from pathlib import Path
from bleak import BleakClient, BleakScanner

DEVICE_NAME="MB2-AI-TIMER"; RX="6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; TX="6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

async def find_device(name, timeout=8.0):
    print(f"Scanning for {name!r}...")
    devices=await BleakScanner.discover(timeout=timeout)
    for d in devices:
        if (d.name or "")==name: return d
    for d in devices:
        if name in (d.name or ""): return d
    return None

def parse_accel(line):
    p=line.strip().split(",")
    if len(p)!=4 or p[0]!="A": return None
    try: return tuple(int(v) for v in p[1:4])
    except ValueError: return None

async def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--name",default=DEVICE_NAME); ap.add_argument("--out",default="exercise_data.csv"); ap.add_argument("--rate",type=int,default=50); args=ap.parse_args()
    path=Path(args.out); new_file=not path.exists() or path.stat().st_size==0
    dev=await find_device(args.name)
    if dev is None: raise SystemExit(f"Could not find BLE device {args.name!r}")
    q=asyncio.Queue(); buf=bytearray()
    def notify(_s,data):
        nonlocal buf
        buf.extend(data)
        while b"\n" in buf:
            raw,_,buf=buf.partition(b"\n"); line=raw.decode("utf-8",errors="replace").strip()
            if line: q.put_nowait(line)
    with path.open("a",newline="",encoding="utf-8") as f:
        w=csv.writer(f)
        if new_file: w.writerow(["utc_time","segment","t_segment_s","label","ax_mg","ay_mg","az_mg","mag_mg"]); f.flush()
        async with BleakClient(dev) as client:
            print(f"Connected: {dev.name} {dev.address}"); await client.start_notify(TX,notify); await client.write_gatt_char(RX,f"R,{args.rate}\n".encode(),response=True); await client.write_gatt_char(RX,b"S,1\n",response=True)
            segment=0; print("Commands: e 30 | r 30 | u 10 | q")
            while True:
                raw=(await asyncio.to_thread(input,"segment> ")).strip().lower()
                if raw=="q": break
                parts=raw.split()
                if len(parts)!=2 or parts[0] not in {"e","r","u"}: print("Use: e 30, r 30, u 10, or q"); continue
                try: duration=float(parts[1])
                except ValueError: continue
                label={"e":"exercise","r":"rest","u":"unknown"}[parts[0]]; segment+=1
                while not q.empty(): q.get_nowait()
                loop=asyncio.get_running_loop(); start=loop.time(); samples=0
                while loop.time()-start<duration:
                    try: line=await asyncio.wait_for(q.get(),timeout=.5)
                    except asyncio.TimeoutError: continue
                    a=parse_accel(line)
                    if a is None: continue
                    x,y,z=a; elapsed=loop.time()-start; mag=math.sqrt(x*x+y*y+z*z)
                    w.writerow([datetime.now(timezone.utc).isoformat(timespec="milliseconds"),segment,f"{elapsed:.6f}",label,x,y,z,f"{mag:.3f}"]); samples+=1
                f.flush(); print(f"Saved {samples} samples to {path}")
            await client.write_gatt_char(RX,b"S,0\n",response=True); await client.stop_notify(TX)

if __name__=="__main__": asyncio.run(main())
