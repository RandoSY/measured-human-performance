#pragma once

#include <stddef.h>
#include <stdint.h>

namespace fft_hrm_sim {

static constexpr uint16_t kMinBpm = 30;
static constexpr uint16_t kMaxBpm = 220;
static constexpr uint16_t kDefaultBpm = 90;

inline uint16_t clampBpm(int value) {
  if (value < static_cast<int>(kMinBpm)) return kMinBpm;
  if (value > static_cast<int>(kMaxBpm)) return kMaxBpm;
  return static_cast<uint16_t>(value);
}

inline uint16_t adjustBpm(uint16_t current, int delta) {
  return clampBpm(static_cast<int>(current) + delta);
}

struct HeartRateMeasurement {
  uint8_t bytes[3] = {0, 0, 0};
  size_t length = 0;
};

// Bluetooth SIG Heart Rate Measurement flags:
// bit 0: 0 = UINT8 BPM, 1 = UINT16 BPM
// bit 1: sensor contact detected
// bit 2: sensor contact supported
inline HeartRateMeasurement makeMeasurement(uint16_t bpm,
                                            bool contactSupported,
                                            bool contactDetected) {
  HeartRateMeasurement packet;
  uint8_t flags = 0;

  if (contactSupported) {
    flags |= 0x04;
    if (contactDetected) flags |= 0x02;
  }

  if (bpm <= 0xFFu) {
    packet.bytes[0] = flags;
    packet.bytes[1] = static_cast<uint8_t>(bpm);
    packet.length = 2;
  } else {
    packet.bytes[0] = static_cast<uint8_t>(flags | 0x01);
    packet.bytes[1] = static_cast<uint8_t>(bpm & 0xFFu);
    packet.bytes[2] = static_cast<uint8_t>((bpm >> 8u) & 0xFFu);
    packet.length = 3;
  }

  return packet;
}

}  // namespace fft_hrm_sim
