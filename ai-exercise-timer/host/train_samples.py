#!/usr/bin/env python3
import argparse, csv
from collections import defaultdict, Counter
from pathlib import Path
import joblib, numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix
from sklearn.model_selection import train_test_split

def features(samples):
    samples=np.asarray(samples,dtype=np.float32); mag=np.sqrt(np.sum(samples*samples,axis=1)); chans={"x":samples[:,0],"y":samples[:,1],"z":samples[:,2],"mag":mag}; vals=[]; names=[]
    for cname,v in chans.items():
        for sname,val in [("mean",np.mean(v)),("std",np.std(v)),("min",np.min(v)),("max",np.max(v)),("range",np.ptp(v)),("rms",np.sqrt(np.mean(v*v)))]: names.append(f"{cname}_{sname}"); vals.append(float(val))
        d=np.diff(v); names.append(f"{cname}_diff_rms"); vals.append(float(np.sqrt(np.mean(d*d))) if len(d) else 0.0)
    return np.asarray(vals,dtype=np.float32), names

def load(path):
    groups=defaultdict(list)
    with Path(path).open(newline="",encoding="utf-8") as f:
        for r in csv.DictReader(f): groups[(r["action"],int(r["sample_id"]))].append([float(r["ax_mg"]),float(r["ay_mg"]),float(r["az_mg"])])
    return groups

def main():
    ap=argparse.ArgumentParser(); ap.add_argument("csv"); ap.add_argument("--out",default="movement_model.joblib"); ap.add_argument("--test-size",type=float,default=.25); args=ap.parse_args()
    X=[]; y=[]; names=None
    for (action,sid),rows in sorted(load(args.csv).items()):
        if len(rows)<5: continue
        feat,names=features(rows); X.append(feat); y.append(action)
    X=np.asarray(X); y=np.asarray(y)
    if len(set(y))<2: raise SystemExit("Need at least two actions.")
    counts=Counter(y)
    if min(counts.values())<3: raise SystemExit("Need at least 3 one-second samples for every action.")
    print("Samples per action:")
    for k in sorted(counts): print(f"  {k}: {counts[k]}")
    if min(counts.values())>=4 and len(y)>=8:
        Xtr,Xte,ytr,yte=train_test_split(X,y,test_size=args.test_size,random_state=42,stratify=y)
        m=RandomForestClassifier(n_estimators=300,max_depth=10,class_weight="balanced",random_state=42); m.fit(Xtr,ytr); pred=m.predict(Xte)
        print(confusion_matrix(yte,pred,labels=sorted(set(y)))); print(classification_report(yte,pred,digits=3,zero_division=0))
    model=RandomForestClassifier(n_estimators=350,max_depth=10,class_weight="balanced",random_state=42); model.fit(X,y)
    joblib.dump({"model":model,"feature_names":names,"actions":list(model.classes_),"sample_seconds":1.0,"recommended_threshold":.70},args.out); print(f"Saved {args.out}")

if __name__=="__main__": main()
