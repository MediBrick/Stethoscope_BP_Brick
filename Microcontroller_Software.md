## Installation
Locate the ["Arduino"](https://github.com/walle637/Stethoscope_BP_Brick/tree/patch-2/Arduino) folder in this repository. Download it to your computer. 

You will need several dependencies to run the Arduino code that is included.
- Dependencies installed directly inside the Arduino IDE (already included in "Arduino\libraries" for your convenience)
    - Adafruit_BusIO
    - Adafruit_GFX_Library
    - Adafruit_MAX1704X
    - Adafruit_MPRLS_Library
    - Adafruit_SSD1306
 - Dependencies downloaded from our provider, pschatzmann, on his GitHub (NOT included in "Arduino\libraries" for credit reasons)
     -  https://github.com/pschatzmann/arduino-audio-tools
     -  https://github.com/pschatzmann/arduino-audio-driver

Do not proceed until these libraries are installed; it will not compile. 

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
