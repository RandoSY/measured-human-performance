#!/usr/bin/env python3
import argparse, asyncio, csv, math
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "MB2-AI-TIMER"
NUS_RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
NUS_TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

def parse_accel(line):
    p=line.strip().split(",")
    if len(p)!=4 or p[0]!="A": return None
    try: return tuple(int(x) for x in p[1:])
    except ValueError: return None

async def find_device(name, timeout=8.0):
    print(f"Scanning for {name!r}...")
    for d in await BleakScanner.discover(timeout=timeout):
        if (d.name or "")==name: return d
    return None

def existing_counts(path):
    c=Counter()
    if not path.exists() or path.stat().st_size==0: return c
    with path.open(newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f): c[row["action"]]=max(c[row["action"]], int(row["sample_id"]))
    return c

async def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--out", default="createai_samples.csv"); ap.add_argument("--name", default=DEVICE_NAME); ap.add_argument("--rate", type=int, default=50); args=ap.parse_args()
    if not 10<=args.rate<=100: raise SystemExit("--rate must be 10..100 Hz")
    path=Path(args.out); new_file=not path.exists() or path.stat().st_size==0; max_ids=existing_counts(path)
    dev=await find_device(args.name)
    if not dev: raise SystemExit(f"Could not find {args.name!r}")
    q=asyncio.Queue(); buf=bytearray()
    def notify(_sender, data):
        nonlocal buf
        buf.extend(data)
        while b"\n" in buf:
            raw,_,buf=buf.partition(b"\n"); s=raw.decode(errors="replace").strip()
            if s: q.put_nowait(s)
    with path.open("a", newline="", encoding="utf-8") as f:
        w=csv.writer(f)
        if new_file: w.writerow(["utc_time","action","sample_id","sample_t_s","ax_mg","ay_mg","az_mg","mag_mg"]); f.flush()
        async with BleakClient(dev) as client:
            print(f"Connected to {dev.name} {dev.address}"); await client.start_notify(NUS_TX_UUID, notify)
            await client.write_gatt_char(NUS_RX_UUID, f"R,{args.rate}\n".encode(), response=True); await client.write_gatt_char(NUS_RX_UUID, b"S,1\n", response=True)
            current="exercise"; print("Commands: add NAME | one | ten | cont10 | list | info | quit")
            async def drain():
                while not q.empty(): q.get_nowait()
            async def capture_one(action, sample_id, duration=1.0):
                await drain(); loop=asyncio.get_running_loop(); t0=loop.time(); rows=[]
                while loop.time()-t0<duration:
                    try: line=await asyncio.wait_for(q.get(), timeout=.35)
                    except asyncio.TimeoutError: continue
                    a=parse_accel(line)
                    if a is None: continue
                    x,y,z=a; elapsed=loop.time()-t0; mag=math.sqrt(x*x+y*y+z*z)
                    rows.append([datetime.now(timezone.utc).isoformat(timespec="milliseconds"),action,sample_id,f"{elapsed:.6f}",x,y,z,f"{mag:.3f}"])
                w.writerows(rows); f.flush(); return len(rows)
            async def capture_ten_continuous(action):
                await drain(); start_id=max_ids[action]+1; loop=asyncio.get_running_loop(); t0=loop.time(); rows=[[] for _ in range(10)]
                while loop.time()-t0<10.0:
                    try: line=await asyncio.wait_for(q.get(), timeout=.35)
                    except asyncio.TimeoutError: continue
                    a=parse_accel(line)
                    if a is None: continue
                    elapsed=loop.time()-t0; sec=min(9,int(elapsed)); x,y,z=a; mag=math.sqrt(x*x+y*y+z*z)
                    rows[sec].append([datetime.now(timezone.utc).isoformat(timespec="milliseconds"),action,start_id+sec,f"{elapsed-sec:.6f}",x,y,z,f"{mag:.3f}"])
                for group in rows: w.writerows(group)
                f.flush(); max_ids[action]+=10; return sum(map(len,rows)), [len(r) for r in rows]
            while True:
                cmd=(await asyncio.to_thread(input, f"{current}> ")).strip(); low=cmd.lower()
                if not cmd: continue
                if low in {"quit","q"}: break
                if low.startswith("add "): current=cmd[4:].strip() or current; print(f"Current action: {current}"); continue
                if low=="list":
                    for a in sorted(max_ids): print(f"{a}: {max_ids[a]} samples")
                    continue
                if low=="info": await client.write_gatt_char(NUS_RX_UUID,b"I\n",response=True); continue
                if low=="one":
                    sid=max_ids[current]+1; n=await capture_one(current,sid); max_ids[current]=sid; print(f"Saved sample {sid}: {n} readings"); continue
                if low=="ten":
                    for _ in range(10):
                        sid=max_ids[current]+1; n=await capture_one(current,sid); max_ids[current]=sid; print(f"Saved sample {sid}: {n} readings"); await asyncio.sleep(.25)
                    continue
                if low=="cont10":
                    n,counts=await capture_ten_continuous(current); print(f"Saved {n} readings; per-sample counts={counts}"); continue
                print("Commands: add NAME | one | ten | cont10 | list | info | quit")
            await client.write_gatt_char(NUS_RX_UUID,b"S,0\n",response=True); await client.stop_notify(NUS_TX_UUID)
    print(f"Saved: {path}")

if __name__=="__main__": asyncio.run(main())
