#!/usr/bin/env python3
import argparse, asyncio, time
from collections import Counter, deque
import joblib, numpy as np
from bleak import BleakClient, BleakScanner
from train import feature_vector

DEVICE_NAME="MB2-AI-TIMER"; RX="6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; TX="6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

def parse_accel(line):
    p=line.strip().split(",")
    if len(p)!=4 or p[0]!="A": return None
    try: return [float(p[1]),float(p[2]),float(p[3])]
    except ValueError: return None

async def find_device(name,timeout=8.0):
    devices=await BleakScanner.discover(timeout=timeout)
    for d in devices:
        if (d.name or "")==name: return d
    return None

class Totals:
    def __init__(self): self.state="unknown"; self.changed=time.monotonic(); self.exercise=0.; self.rest=0.
    def transition(self,new):
        now=time.monotonic(); elapsed=now-self.changed
        if self.state=="exercise": self.exercise+=elapsed
        elif self.state=="rest": self.rest+=elapsed
        self.state=new; self.changed=now
    def snapshot(self):
        ex,rest=self.exercise,self.rest; elapsed=time.monotonic()-self.changed
        if self.state=="exercise": ex+=elapsed
        elif self.state=="rest": rest+=elapsed
        return ex,rest

async def main():
    ap=argparse.ArgumentParser(); ap.add_argument("--model",default="exercise_model.joblib"); ap.add_argument("--name",default=DEVICE_NAME); args=ap.parse_args()
    pkg=joblib.load(args.model); model=pkg["model"]; rate=int(pkg["sample_rate_hz"]); window_n=int(pkg["window_n"]); step_n=int(pkg["step_n"])
    dev=await find_device(args.name)
    if dev is None: raise SystemExit("micro:bit not found")
    q=asyncio.Queue(); buf=bytearray()
    def notify(_s,data):
        nonlocal buf
        buf.extend(data)
        while b"\n" in buf:
            raw,_,buf=buf.partition(b"\n"); q.put_nowait(raw.decode(errors="replace").strip())
    samples=deque(maxlen=window_n); decisions=deque(maxlen=3); since=0; totals=Totals()
    async with BleakClient(dev) as client:
        await client.start_notify(TX,notify); await client.write_gatt_char(RX,f"R,{rate}\n".encode(),response=True); await client.write_gatt_char(RX,b"S,1\n",response=True)
        while True:
            a=parse_accel(await q.get())
            if a is None: continue
            samples.append(a); since+=1
            if len(samples)<window_n or since<step_n: continue
            since=0; feat,_=feature_vector(np.asarray(samples,dtype=np.float32)); probs=model.predict_proba(feat.reshape(1,-1))[0]; i=int(np.argmax(probs)); state=model.classes_[i]; decisions.append(state); smooth=Counter(decisions).most_common(1)[0][0]; totals.transition(smooth); ex,rest=totals.snapshot(); print(f"\rstate={smooth:8s} p={probs[i]:.2f} exercise={ex:7.1f}s rest={rest:7.1f}s",end="",flush=True)

if __name__=="__main__":
    try: asyncio.run(main())
    except KeyboardInterrupt: print()
