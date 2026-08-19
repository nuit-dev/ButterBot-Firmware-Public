# ButterBot Firmware

## Building

To build the ButterBot firmware, you'll need the ESP-IDF. You can find the getting started
guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/).
The production firmware is built using IDF version 5.5.3.

All required components are contained in the repository, and managed components (esp-sr,
esp-dl, esp-tflite-micro, face detection and recognition models, etc.) are fetched
automatically by the IDF component manager during the build.

In the root directory of the project:

**To build the firmware** run ```idf.py build```

**To upload the firmware to the device** run ```idf.py -p <PORT> flash```. Replace `<PORT>` with
the port the robot is attached to, for ex. ```COM6``` or ```/dev/ttyACM0```.

Flashing also uploads the data partitions: the SPIFFS filesystem image built from
[spiffs](spiffs), the object detection model from [models](models), and the speech
recognition and face detection / recognition models provided by the managed components.

## Adding a custom voice

The robot talks using [flite](https://github.com/festvox/flite) text-to-speech, vendored
in [components/flite](components/flite). You can swap the stock voice for any other flite
voice — both diphone and ClusterGen (CG) voices are supported. The only hard constraint is
that the voice must be **16 kHz**, since the whole audio chain (I2S output, effects) is
hardwired to that sample rate.

You can use one of the prebuilt voices from the
[flite repository](https://github.com/festvox/flite) (see `src/lang/`), or build your own
with the [FestVox](http://festvox.org/) tools and export it to C sources using flite's
`flitevox` conversion tools.

To add a voice (for example `cmu_us_xxx`):

1. Copy the voice's generated C sources into `components/flite/src/lang/cmu_us_xxx/` and
   its `voxdefs.h` into `components/flite/include/lang/cmu_us_xxx/`. No edits needed — the
   build picks the sources up automatically.
2. Swap the voice registration in [main/src/Audio/SpeechGen.cpp](main/src/Audio/SpeechGen.cpp)
   (three lines): the `extern "C"` declarations at the top, and the `register_cmu_us_xxx` /
   `unregister_cmu_us_xxx` calls in the constructor and destructor.
3. Build and flash.

Note: CG voices run the MLSA vocoder, which in this port has been converted to
single-precision floating point (`components/flite/src/cg/cst_mlsa.c`) — the ESP32-S3 FPU
is single-precision only, and double math would make synthesis slower than real-time.

## Restoring the stock firmware

To restore the stock firmware, you can download the prebuilt binary on
the [releases page](https://github.com/CircuitMess/ButterBot-Firmware-Public/releases) of
this repository and flash it manually using esptool:

```shell
esptool -c esp32s3 -b 921600 -p <PORT> write_flash 0 ButterBot-Firmware.bin
```

Alternatively, you can also do so using [CircuitBlocks](https://code.circuitmess.com/) by
logging in, clicking the "Restore Firmware" button in the top-right corner, and following the
on-screen instructions.
