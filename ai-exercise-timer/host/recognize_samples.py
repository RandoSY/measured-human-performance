#!/usr/bin/env python3
import argparse, asyncio
from collections import deque, Counter
import joblib, numpy as np
from bleak import BleakClient, BleakScanner
from train_samples import features

DEVICE_NAME="MB2-AI-TIMER"; RX="6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; TX="6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

def label_char(label): return next((c.upper() for c in label if c.isalnum()),"?")
async def display(client,mode,label=None):
    cmd=f"D,{mode},{label_char(label)}\n" if label else f"D,{mode}\n"
    try: await client.write_gatt_char(RX,cmd.encode(),response=True)
    except Exception: pass

def parse(line):
    p=line.strip().split(",")
    if len(p)!=4 or p[0]!="A": return None
    try: return [float(p[1]),float(p[2]),float(p[3])]
    except ValueError: return None

async def find(name):
    devices = await BleakScanner.discover(timeout=8)
    for d in devices:
        if (d.name or "") == name:
            return d
    for d in devices:
        if (d.name or "").startswith(name):
            return d
    return None

async def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--model",default="movement_model.joblib"); ap.add_argument("--name",default=DEVICE_NAME); ap.add_argument("--rate",type=int,default=50); ap.add_argument("--threshold",type=float,default=None); args=ap.parse_args()
    pkg=joblib.load(args.model); model=pkg["model"]; threshold=args.threshold if args.threshold is not None else pkg.get("recommended_threshold",.70); window_n=args.rate
    dev=await find(args.name)
    if not dev: raise SystemExit("micro:bit not found")
    q=asyncio.Queue(); buf=bytearray()
    def notify(_s,data):
        nonlocal buf
        buf.extend(data)
        while b"\n" in buf:
            raw,_,buf=buf.partition(b"\n"); q.put_nowait(raw.decode(errors="replace").strip())
    samples=deque(maxlen=window_n); decisions=deque(maxlen=3); since=0; last_state=None
    async with BleakClient(dev) as client:
        await client.start_notify(TX,notify); await client.write_gatt_char(RX,f"R,{args.rate}\n".encode(),response=True); await client.write_gatt_char(RX,b"S,1\n",response=True); await display(client,"STREAM")
        print(f"Recognition point: {threshold:.0%}")
        try:
            while True:
                a=parse(await q.get())
                if a is None: continue
                samples.append(a); since+=1
                if len(samples)<window_n or since<max(1,args.rate//4): continue
                since=0; feat,_=features(np.asarray(samples,dtype=np.float32)); probs=model.predict_proba(feat.reshape(1,-1))[0]; i=int(np.argmax(probs)); certainty=float(probs[i]); raw_state=model.classes_[i] if certainty>=threshold else "unknown"; decisions.append(raw_state); state=Counter(decisions).most_common(1)[0][0]
                if state!=last_state:
                    await display(client,"UNKNOWN" if state=="unknown" else "CLASS",None if state=="unknown" else state)
                    if last_state is not None: print(f"STOP  {last_state}")
                    print(f"START {state}"); last_state=state
                allp=" ".join(f"{c}={p:.0%}" for c,p in zip(model.classes_,probs)); print(f"\r{state:12s} best={certainty:.0%}  {allp}   ",end="",flush=True)
        finally:
            await display(client,"STOP")
            try: await client.write_gatt_char(RX,b"S,0\n",response=True)
            except Exception: pass

if __name__=="__main__":
    try: asyncio.run(main())
    except KeyboardInterrupt: print()
