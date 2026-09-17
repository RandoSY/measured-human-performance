#!/usr/bin/env python3
import argparse, csv
from collections import defaultdict
from pathlib import Path
import joblib, numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, confusion_matrix, accuracy_score

AXES=("x","y","z","mag")

def feature_vector(samples):
    mag=np.sqrt(np.sum(samples*samples,axis=1)); channels={"x":samples[:,0],"y":samples[:,1],"z":samples[:,2],"mag":mag}; values=[]; names=[]
    for name in AXES:
        v=channels[name]
        for stat,val in [("mean",np.mean(v)),("std",np.std(v)),("min",np.min(v)),("max",np.max(v)),("rms",np.sqrt(np.mean(v*v)))]: names.append(f"{name}_{stat}"); values.append(float(val))
    for name in AXES:
        d=np.diff(channels[name]); names.append(f"{name}_diff_rms"); values.append(float(np.sqrt(np.mean(d*d))) if len(d) else 0.0)
    return np.asarray(values,dtype=np.float32), names

def load_segments(path):
    segments=defaultdict(list); labels={}
    with path.open(newline="",encoding="utf-8") as f:
        for row in csv.DictReader(f):
            label=row["label"].strip().lower()
            if label not in {"exercise","rest"}: continue
            seg=int(row["segment"]); labels[seg]=label; segments[seg].append([float(row["ax_mg"]),float(row["ay_mg"]),float(row["az_mg"])])
    return {k:np.asarray(v,dtype=np.float32) for k,v in segments.items()}, labels

def make_windows(segments,labels,window_n,step_n):
    X=[]; y=[]; groups=[]; names=None
    for seg in sorted(segments):
        arr=segments[seg]
        for start in range(0,max(0,len(arr)-window_n+1),step_n):
            feat,names=feature_vector(arr[start:start+window_n]); X.append(feat); y.append(labels[seg]); groups.append(seg)
    if not X: raise SystemExit("No complete windows.")
    return np.vstack(X),np.asarray(y),np.asarray(groups),names

def main():
    ap=argparse.ArgumentParser(); ap.add_argument("csv"); ap.add_argument("--out",default="exercise_model.joblib"); ap.add_argument("--rate",type=int,default=50); ap.add_argument("--window",type=float,default=1.0); ap.add_argument("--stride",type=float,default=.5); args=ap.parse_args()
    segs,labels=load_segments(Path(args.csv)); X,y,groups,names=make_windows(segs,labels,max(5,round(args.rate*args.window)),max(1,round(args.rate*args.stride)))
    hold=[]
    for label in ("exercise","rest"):
        s=sorted(set(groups[y==label]));
        if len(s)>=2: hold.append(s[-1])
    if len(hold)==2:
        test=np.isin(groups,hold); m=RandomForestClassifier(n_estimators=200,max_depth=8,random_state=42,class_weight="balanced"); m.fit(X[~test],y[~test]); pred=m.predict(X[test]); print(f"Accuracy: {accuracy_score(y[test],pred):.3f}"); print(confusion_matrix(y[test],pred,labels=["exercise","rest"])); print(classification_report(y[test],pred,digits=3,zero_division=0))
    model=RandomForestClassifier(n_estimators=250,max_depth=8,random_state=42,class_weight="balanced"); model.fit(X,y)
    joblib.dump({"model":model,"feature_names":names,"sample_rate_hz":args.rate,"window_n":max(5,round(args.rate*args.window)),"step_n":max(1,round(args.rate*args.stride)),"labels":list(model.classes_)},args.out); print(f"Saved model: {args.out}")

if __name__=="__main__": main()
