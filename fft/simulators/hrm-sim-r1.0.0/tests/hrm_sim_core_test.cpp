#include <cassert>
#include <cstdint>
#include "hrm_sim_core.h"

int main() {
  using namespace fft_hrm_sim;
  assert(clampBpm(1) == 30);
  assert(clampBpm(90) == 90);
  assert(clampBpm(999) == 220);
  assert(adjustBpm(30, -1) == 30);
  assert(adjustBpm(220, 1) == 220);

  HeartRateMeasurement p = makeMeasurement(90, true, true);
  assert(p.length == 2);
  assert(p.bytes[0] == 0x06);
  assert(p.bytes[1] == 90);

  p = makeMeasurement(110, true, false);
  assert(p.length == 2);
  assert(p.bytes[0] == 0x04);
  assert(p.bytes[1] == 110);

  p = makeMeasurement(300, false, false);
  assert(p.length == 3);
  assert(p.bytes[0] == 0x01);
  assert(p.bytes[1] == 0x2C);
  assert(p.bytes[2] == 0x01);
  return 0;
}
