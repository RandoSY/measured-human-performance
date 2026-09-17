# Teaching notes

The full chain stays visible:

`measurement -> transport -> labeling -> features -> model -> certainty -> decision`

A useful sequence is:

1. Measure real x/y/z acceleration.
2. Label one-second examples.
3. Inspect bad or unrepresentative samples.
4. Convert raw time series into understandable features.
5. Train a classifier.
6. Test held-out samples.
7. Recognize live movement.
8. Permit `unknown` when certainty is insufficient.
9. Deliberately find failures.
10. Improve the data/model—or decide a conventional algorithm is enough.

Questions worth asking:

- How many samples are enough?
- Does another person get the same result?
- Does wrist orientation matter?
- Which feature actually separates the classes?
- What changes when the threshold moves from 0.5 to 0.7 to 0.9?
- Can acceleration-magnitude variance solve the problem without ML?
- At what point does ML earn its complexity?

A useful baseline is vector magnitude:

`m = sqrt(x^2 + y^2 + z^2)`

Then compare the standard deviation or RMS of changes in `m` over one second with the ML result.

Preserve raw data. A classification is an interpretation; the accelerometer measurements are the observations that produced it.
