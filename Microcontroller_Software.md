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

## Usage Guide
The microcontroller software includes several built-in functions, including:

- Auto-shutoff after 4 hours
- Automatic zeroing when the MediBrick is turned on
- Blood pressure readings
- Stethoscope audio output
   - Headphone jack audio
   - SerialUI plotter

By default, the MediBrick operates in both blood pressure and stethoscope mode. In this mode, pressure readings are shown on the OLED display while stethoscope audio remains active. The stethoscope volume is set to 80% by default, and the SerialUI audio plot is disabled.

These settings can be adjusted through the internal serial menu. To access the menu guide, connect the MediBrick to the Arduino IDE through USB, open the Serial Monitor, and enter `h`. The Serial Monitor will then print the menu on the screen.

The following commands can be entered into the Serial Monitor:

| Command | Function |
|---|---|
| `p` | Pressure-only mode |
| `s` | Stethoscope-only mode |
| `b` | Both: pressure on OLED and audio active |
| `t` | Toggle the stethoscope audio plot to SerialUI on/off |
| `h` | Help |
| `<` | Decrease output volume |
| `>` | Increase output volume |
| `v` | Print current output volume |
