# Microphones

## Electret

- Primo EM272Z1 80 SNR -28dB 60-20000 10mm $17
- Primo EM272J  80 SNR -28dB 60-20000 10mm 600uA $17 digikey
- Primo EM258   75 SNR -32dB 60-20000 5.8mm $4.73
- Primo EM281   71 SNR -39dB 30-20000 5.8mm 450 uA $8.24 digikey
- PUI AUM-4538L 60 SNR -38dB 20-20000 9.7mm 450 uA $3 digikey

** PUI AOM-5024L-HD-R 80 SNR -24dB 20-20000 10mm (4mm opening) $4.2 **

## MEMS
differential, needs differential amp (Adafruit reference design)

- Infineon       IM70A135  70 SNR -38dB 37-20000 170uA $2.25
- TDK/InvenSense ICS-40638 63 SNR -43dB 35-20000 170uA $3.41 digikey

## Current MEMS
- PUI         AMM-2742-T-R 59 SNR -42dB 20-20000 $1.8 digikey

## Circuit for New PUI AOM-5024L-HD-R or PUI AOM-5024L-HD-F-R

### Microphone Connection
```
3.3V_Analog
  |
  2.2k
  |
  +--------- MIC+
  |
  +--------- audio out node
            |
           Ccouple 4.7 -10 uF
            |
           next stage

MIC- ---------------- AGND
```

### Direct connection (not used)
```
Electret bias node
      |
      Ccouple
   4.7–10 µF
      |
     470-1k
      |
      +-------- ES8388 LIN/RIN
      |
     1 nF (470-2.2nF)
      |
     AGND
```

### With Non Inverting OpAmp (not used)

```
3.3V_A
  |
 47k
  |
  +-----+----+- Vmid ≈ 1.65 V
  |     |    |
 47k  1-10uF 100nF
  |     |    |
 AGND   AGND AGND


Electret bias node
      |
   Ccouple
     4.7 µF
      |
      +---- 10k ----> Op Amp +IN
                    |
                    1M
                    |
                   Vmid


                 Rf
OpAmp OUT -----/\/\/\-----+
                          |
                        OpAmp -IN
                          |
                 Rg       |
Vmid ----------/\/\/\-----+

Gain = 1 + Rf/Rg

2× gain:  Rf = 10 kΩ, Rg = 10 kΩ
5× gain:  Rf = 39 kΩ, Rg = 10 kΩ
10× gain: Rf = 90 kΩ, Rg = 10 kΩ
```
## With Inverting OpAmp (used)

### Bias
```
3.3V_A
  |
 47k
  |
  +-----+----+- Vmid ≈ 1.65 V
  |     |    |
 47k  1-10uF 100nF
  |     |    |
 AGND   AGND AGND
```
### Electret microphone bias
```
3.3V_A
  |
 2.2k
  |
  +-------- Electret Mic +
  |
  +-------- Electret bias / signal node
```
### Coupling into inverting input
```
Electret bias / signal node
      |
   Ccouple
   4.7 µF
      |
      +---- Rin ----> Op Amp -IN

Op Amp +IN --------> Vmid
```

Optional bleed/startup resistor:

```
Node after Ccouple
      |
      1M
      |
     Vmid
```

### Fedback

```
                 Rf
OpAmp OUT -----/\/\/\-----+
                 ||       |
                 Cf       |
                 ||       |
                        OpAmp -IN
                          |
                 Rin      |
Mic signal ----/\/\/\-----+

Gain = -Rf / Rin

1× gain:   Rf = 10 kΩ,   Rin = 10 kΩ
2× gain:   Rf = 22 kΩ,   Rin = 10 kΩ **
5× gain:   Rf = 49.9 kΩ, Rin = 10 kΩ
10× gain:  Rf = 100 kΩ,  Rin = 10 kΩ

Rf = 10 kΩ,   Cf = 1 nF   → fc ≈ 15.9 kHz **
Rf = 22 kΩ,   Cf = 1 nF   → fc ≈ 7.2 kHz
Rf = 49.9 kΩ, Cf = 470 pF → fc ≈ 6.8 kHz
```
