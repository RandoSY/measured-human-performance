from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MAIN = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
PIO = (ROOT / "platformio.ini").read_text(encoding="utf-8")


def test_standard_heart_rate_profile_present():
    assert 'DEVICE_NAME = "FFT-HRM-SIM"' in MAIN
    assert 'HRS_UUID = "180D"' in MAIN
    assert 'HRM_UUID = "2A37"' in MAIN
    assert 'BODY_SENSOR_LOCATION_UUID = "2A38"' in MAIN
    assert "NIMBLE_PROPERTY::NOTIFY" in MAIN


def test_buttons_and_reset_present():
    assert "FFT_HRM_BUTTON_A_PIN" in MAIN
    assert "FFT_HRM_BUTTON_B_PIN" in MAIN
    assert '"A+B_RESET"' in MAIN
    assert "kDefaultBpm" in MAIN


def test_advertising_name_order_is_safe():
    enable = MAIN.index("advertising->enableScanResponse(true)")
    name = MAIN.index("advertising->setName(DEVICE_NAME)")
    start = MAIN.index("advertising->start()")
    assert enable < name < start


def test_stack_and_matrix_configuration():
    assert "CONFIG_MAIN_TASK_STACK_SIZE=1024" in PIO
    assert "Adafruit Microbit Library@1.3.4" in PIO
    assert "bbcmicrobitv2" in PIO
    assert "fft_hrm_sim_microbit_v2" in PIO


def test_measurement_is_sent_directly_without_large_stack_format_buffer():
    assert "g_hrMeasurement->notify(packet.bytes, packet.length" in MAIN
    assert not re.search(r"char\s+\w+\s*\[\s*[5-9]\d{2,}", MAIN)
