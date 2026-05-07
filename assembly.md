# Assembly of Stethoscope & Blood Pressure MediBrick

## Required Parts

- Fully assembled Stethoscope PCB
- Microcontroller board:
  - Adafruit Feather ESP32-S3, or
  - SparkFun Thing Plus C
- Honeywell MPRLS0300YG pressure sensor
- OLED display
- Pushbutton
- Battery
- AWG 22 or AWG 24 silicone wire
- Stainless steel tubing, 7 mm OD, 5 mm ID
- Stethoscope chest piece and tubing
- Inflatable arm cuff
- Silicone tubing
- Luer lock adapters
- Miscellaneous hardware:
  - M3 screws, 20 mm, and nuts for PCB mounting
  - M3 screws and nuts for microcontroller / enclosure mounting

## 3D Printed Parts

For the design models, refer to the README.

- MediBrick Outer Casing
  - Outer casing top part
  - Outer casing bottom part
- Microphone Housing
  - Microphone housing top part
  - Microphone housing bottom part
- Side plate

## Tubing and Adapter Notes

- Suggested stethoscope rubber tube: **5 mm ID**, using a barb with **8 mm OD**
- Suggested pressure cuff tube: **4 mm ID**, using a barb with **7 mm OD**
- Rubber tubing from Honeywell pressure sensor to arm cuff: **2 mm ID, 4 mm OD**
- Luer Lock **3/32** barbed adapter for rubber tubing from pressure sensor
- Luer Lock **5/32** barbed adapter for pressure cuff tube

## Required Tools

- Soldering iron
- Hot-air rework station
- Solder
- Flux
- Tweezers
- Isopropyl alcohol
- Small screwdrivers
- Wire cutters / wire strippers
- Continuity tester or multimeter

---

# Assembly Procedure

## 1. Collect and inspect all parts

Verify that all electrical, mechanical, and tubing components are present before beginning assembly. Inspect the PCB, 3D-printed parts, and microcontroller board for visible damage or manufacturing defects.

## 2. Solder the pressure sensor to the PCB

Use a hot-air rework station to solder the **Honeywell MPRLS0300YG** pressure sensor to the board.

1. Locate the pressure sensor footprint on the PCB. The solder pads form a square pattern.
2. Identify the orientation marker on the PCB footprint. One corner of the footprint has a dot.
3. Apply flux to the pressure sensor pads on the PCB. A generous amount of flux is acceptable.
4. Inspect the underside of the pressure sensor and locate its orientation dot.
5. Place the pressure sensor onto the PCB footprint, aligning the dot on the sensor with the dot on the PCB.
6. Set the hot-air station to approximately **350 °C**.
7. Using gentle airflow, heat the pressure sensor evenly from above until the solder reflows and the sensor settles into place.
8. Do **not** push the sensor while the solder is molten.
9. Remove the hot air and allow the board to cool before touching or moving the sensor.
10. Clean the flux residue using isopropyl alcohol.

> High-quality hot-air stations are available in the Engineering Design Center electronics room. Supervision is strongly recommended if you are not familiar with hot-air soldering.

## 3. Attach wires to the PCB and microcontroller

Attach color-coded wires to the PCB I/O pads. A suggested color convention is:

- **Red** for power
- **Black** or **green** for ground
- **Blue** or **white** for digital input/output
- **Yellow** for analog wires

Wires may be inserted into the pad holes or soldered perpendicularly onto the pads. Route all wires carefully and avoid excessive strain on the solder joints.

> The PCB is vulnerable to shorts, so inspect closely for solder bridges or unintended conductive paths before power-up.

---

# Electrical Connections

## Main PCB Connections

Suggested connections for the SparkFun Thing Plus C (USB-C) and Adafruit Feather ESP32-S3 are given below.

| PAD | Function | Thing Plus | Feather |
|---|---|---|---|
| **GND** | Ground | GND | GND |
| **3.3V** | Power | 3V3 | 3V3 |
| **SDA** | SDA / CDATA | SDA / GPIO21 | SDA / GPIO3 |
| **SCL** | SCL / CCLK | SCL / GPIO22 | SCL / GPIO4 |
| **DOUT** | Data Out | POCI / GPIO19 | MISO / GPIO37 |
| **LRCLK** | WS / Word Clock | A5 / GPIO35 | A5 / GPIO8 / ADC1-CH7 |
| **DIN** | Data In / DSDIN | PICO / GPIO23 | MOSI / GPIO35 |
| **SLCK** | Bit Clock | SCK / GPIO18 | SCK / GPIO36 |
| **MCLK** | Master Clock | LED / GPIO13 | A4 / GPIO14 / ADC2-CH3 |
| **3.3V** | Power | 3V3 | 3V3 |

Required connections are shown in **bold**.

## Button

| PAD | Function | Thing Plus | Feather |
|---|---|---|---|
| **3.3V** | Power | 3V3 | 3V3 |
| **Button** | Button signal | 9 or A0 | 12 |

## OLED Display

| PAD | Function | Thing Plus | Feather |
|---|---|---|---|
| **VCC** | Power |  | 3V3 |
| **GND** | Ground |  | GND |
| **SDA** | I2C Data |  | 3 |
| **SCL** | I2C Clock |  | 4 |

## Pressure Sensor Control Lines

The MPRLS0300YG also requires two signal wires to the ESP32-S3 board.

| PAD | Function | Thing Plus | Feather |
|---|---|---|---|
| **EOC** | End of Conversion |  | D10 |
| **RST** | Reset |  | D11 |

---

# Mechanical Assembly

## 4. Mount the PCB into the enclosure

Place the custom PCB into the **bottom portion of the outer casing** and secure it using the appropriate M3 screws and nuts. Make sure the board is seated properly and that no wires are trapped beneath it.

## 5. Mount the microcontroller board

Position the microcontroller board in its designated location within the enclosure and secure it with the appropriate hardware. Route the wires neatly so they do not place excessive strain on the solder joints.

## 6. Assemble and install the microphone housing

Attach the microphone housing top and bottom parts and secure them with the required screws. Ensure that the housing is aligned correctly and that the PCB-mounted microphones remain unobstructed.

## 7. Insert the stainless steel tube into the microphone housing

Place the stainless steel tube into the microphone housing so that it forms the acoustic path between the stethoscope tubing and the microphone assembly. Verify that the tube is seated securely.

## 8. Connect the stethoscope tubing

Attach the stethoscope tubing to the microphone housing interface. Ensure a secure fit, since poor acoustic coupling may reduce heartbeat audibility.

## 9. Connect the pressure tubing

Attach the silicone tubing and luer lock adapters to connect the pressure cuff path to the Honeywell pressure sensor.

- Use the **2 mm ID / 4 mm OD** tubing segment for the pressure sensor connection
- Use the correct luer lock adapters for the pressure sensor and cuff tubing
- Verify that all pressure connections are secure and free from leaks

## 10. Install the display, button, and battery

Mount the OLED display, pushbutton, and battery according to the final enclosure layout. Confirm that:

- the display is visible through the enclosure opening
- the pushbutton is accessible
- the battery is seated securely

## 11. Close the enclosure

Install the **top portion of the outer casing** and secure it to the bottom portion. During closure, make sure no wires or tubing are pinched and that all components remain properly seated.

---

# Final Inspection and Power-Up

## 12. Perform final inspection

Before applying power:

- inspect the PCB for shorts or solder bridges
- verify all wiring against the pin tables above
- confirm tubing paths are secure
- check mounting hardware
- ensure the enclosure closes cleanly without pinching wires or tubing

## 13. Power on and verify basic functionality

Apply power to the device and confirm successful startup. Verify:

- OLED operation
- pressure sensing
- audio output
- button response

After basic functionality is confirmed, the device is ready for formal verification testing.

---

# Pinout References

- [SparkFun Thing Plus C Pinout](https://cdn.sparkfun.com/assets/3/9/5/f/e/SparkFun_Thing_Plus_ESP32_WROOM_C_graphical_datasheet2.pdf)
- [Adafruit ESP32-S3 Pinout](https://learn.adafruit.com/assets/110811)
