## Installation
Locate the "Arduino" folder in this repository. Download it to your computer. 

Install the following third-party libraries before compiling the software.

In the Arduino IDE, open `Sketch -> Include Library -> Manage Libraries...`, then search for and install each library listed below.

- [Adafruit BusIO](https://github.com/adafruit/Adafruit_BusIO)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit MAX1704X](https://github.com/adafruit/Adafruit_MAX1704X)
- [Adafruit MPRLS Library](https://github.com/adafruit/Adafruit_MPRLS)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

- Dependencies downloaded from pschatzmann, on his GitHub
  -  https://github.com/pschatzmann/arduino-audio-tools
  -  https://github.com/pschatzmann/arduino-audio-driver

Using the Arduino IDE, open the "MediBrick_Stethoscope_BP_Microcontroller_Software" folder, and flash the code to your ESP32-S3.

You will need to make sure the defaults for the wiring are correct.
It is recommended to select board configuration "USB Mode: CDC", "Upload Hardware CDC" and to enable "CDC on Boot".

## Usage Guide
The microcontroller software includes several built-in functions, including:

- Auto-shutoff after 4.5 hours
- Automatic zeroing of pressure when the MediBrick is turned on
- Blood pressure readings
- Stethoscope audio output
   - Headphone jack audio
   - SerialUI plotter

By default, the MediBrick operates in both blood pressure and stethoscope mode. In this mode, pressure readings are shown on the OLED display while stethoscope audio remains active. The stethoscope volume is set to 80% by default, and the SerialUI audio plot is disabled.

These settings can be adjusted through the internal serial menu. To access the menu guide, connect the MediBrick to the Arduino IDE through USB, open the Serial Monitor, and enter `h`. The Serial Monitor will then print the menu on the screen.

The following commands can be entered into the Serial Monitor:

**Mode selection**

| Command | Function |
|---|---|
| `p` | Pressure-only mode |
| `s` | Stethoscope-only mode |
| `b` | Both: pressure on OLED and audio active |

**Serial plot control** — while the plot is active, command responses are suppressed to keep the data stream clean

| Command | Function |
|---|---|
| `t` | Toggle audio plot on/off |
| `d` | Cycle plot decimation: 1 → 2 → 4 → 8 → 1 (reduces sample rate sent over serial) |
| `?` | Print help and turn plot off |
| `v` | Print all current settings and turn plot off |

**Settings persistence**

| Command | Function |
|---|---|
| `j` | Save current settings to flash |
| `J` | Load settings from flash and apply |

**Output (codec / headphone) volume** — range 0–100, step 6

| Command | Function |
|---|---|
| `>` | Output volume up |
| `<` | Output volume down |

**Input (microphone) volume** — nine discrete 3 dB gain levels

| Command | Function |
|---|---|
| `.` | Input volume up |
| `,` | Input volume down |

**Software gain** — range 0.5–4.0, step 0.5

| Command | Function |
|---|---|
| `+` | Software gain up |
| `-` | Software gain down |

**Low-pass filter** — step 250 Hz

| Command | Function |
|---|---|
| `L` | Low-pass cutoff frequency up |
| `l` | Low-pass cutoff frequency down |
| `K` | Low-pass filter ON |
| `k` | Low-pass filter OFF |

**High-pass filter** — step 1 Hz

| Command | Function |
|---|---|
| `H` | High-pass cutoff frequency up |
| `h` | High-pass cutoff frequency down |
| `G` | High-pass filter ON |
| `g` | High-pass filter OFF |

**Noise cancellation** (right channel minus scaled left channel)

| Command | Function |
|---|---|
| `N` | Noise cancel ON |
| `n` | Noise cancel OFF |
| `C` | Left noise scale up (step 0.05, range 0.5–1.5) |
| `c` | Left noise scale down (step 0.05, range 0.5–1.5) |
