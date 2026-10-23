# ModelW Precision Dynamics Processor

**ModelW** is a professional stereo dynamics and relative spectral de-essing audio plugin designed for transparent mastering and high-energy mix bus compression.

---

## Key Features & DSP Architecture

1. **Log-Domain Feed-Forward Gain Computer**
   - Precise 2nd-order quadratic soft knee curve providing smooth, continuous transition without abrupt threshold discontinuities.
   - Threshold range: `-60.0 dB` to `0.0 dB`.
   - Compression ratio: `1.0:1` to `64.0:1`.
   - Continuous soft knee width control: `0.0 dB` to `40.0 dB`.

2. **Relative Spectral Tilt De-Essing**
   - Evaluates high-frequency sibilance relative to instantaneous broadband energy rather than static absolute amplitude.
   - Eliminates over-compression on quiet or dark vocal passages while keeping harsh sibilants precisely in check.
   - Dynamic subtractive biquad crossover maintains zero phase distortion outside the detection band.
   - Real-time sidechain audition / monitor mode.

3. **Dual-Slope Ballistics & Transient Hold**
   - Attack times down to `0.1 ms` for fast transient capture.
   - Dedicated Hold stage (`0.0 ms` to `100.0 ms`) prevents low-frequency waveform modulation and bass flutter.
   - Dual-slope release curve adapts smoothly to musical dynamics.

4. **Advanced Engine Controls**
   - **SoftSat Crossover:** Adds subtle harmonic saturation across the crossover band.
   - **L/R Input & Output Differential Trim:** Micro-balance stereo adjustments (`±6.0 dB`).
   - **VCA Ballistic Smoothing:** Fine-tunes the smoothing time constant of the gain reduction path (`0.1 ms` to `10.0 ms`).
   - **Engine Subsampling:** Selectable control-rate execution (`1x Full Rate`, `2x`, `4x`, `8x`).

5. **Modern Tabbed Interface (560x560 Square Window)**
   - **Dynamics Tab:** Real-time transfer function curve displaying the quadratic soft knee transition and live operating point tracker dot.
   - **De-Esser Tab:** Real-time spectral crossover canvas and sidechain auditioning.
   - **Advanced Tab:** Direct access to engine calibration and fine-tuning parameters.
   - **Metering Bar:** Fast stereo peak input/output metering and gain reduction readout.

---

## Supported Platforms & Formats

- **Windows (64-bit):** VST3, Standalone
- **macOS (Apple Silicon & Intel Universal):** AudioUnit (AU / `.component`), VST3, Standalone

---

## Building from Source

ModelW uses **CMake** and fetches the **JUCE Framework** automatically during configuration. No manual SDK installation is required.

### Prerequisites

- **CMake** 3.22 or higher
- **Git**
- **Windows:** Visual Studio 2022 / 2019 / Build Tools with C++20 support
- **macOS:** Xcode or Command Line Tools (Apple Clang)

---

### Windows (64-bit VST3 & Standalone)

Run the included automated build script from the project root:

```cmd
Scripts\build_win64.bat
```

Or configure and build manually via CMake:

```cmd
cmake -B build_win64 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build_win64 --config Release
```

The compiled binaries will be generated at:
- `build_win64/ModelW_artefacts/Release/VST3/ModelW.vst3`
- `build_win64/ModelW_artefacts/Release/Standalone/ModelW.exe`

---

### macOS (Apple Silicon M-Series AU & VST3)

Run the included build script:

```bash
chmod +x Scripts/build_mac.sh
./Scripts/build_mac.sh
```

Or build manually with universal ARM64/x86_64 target:

```bash
cmake -B build_mac -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build_mac --config Release
```

The compiled plugins will be generated at:
- `build_mac/ModelW_artefacts/Release/AU/ModelW.component`
- `build_mac/ModelW_artefacts/Release/VST3/ModelW.vst3`
- `build_mac/ModelW_artefacts/Release/Standalone/ModelW.app`

To install on macOS:
```bash
cp -R build_mac/ModelW_artefacts/Release/AU/ModelW.component ~/Library/Audio/Plug-Ins/Components/
cp -R build_mac/ModelW_artefacts/Release/VST3/ModelW.vst3 ~/Library/Audio/Plug-Ins/VST3/
killall -9 AudioComponentRegistrar
```

---

## License

Copyright © ModelW Audio. All rights reserved.
