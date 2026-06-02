/*
  MediBrick Stethoscope + Blood Pressure
  ESP32-S3 Feather

  Runtime Serial Commands:
    p = Pressure only
    s = Stethoscope only
    b = Both (pressure on OLED + audio active)
    h = Help
    < = output volume down
    > = output volume up
    v = print output volume

  Default boot mode:
    BOTH
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPRLS.h>
#include "driver/rtc_io.h"
#include "driver/i2s.h"
#include "AudioBoard.h"
#include <Adafruit_MAX1704X.h>

// ===================== APP MODES =====================
#define APP_MODE_PRESSURE     1
#define APP_MODE_STETHOSCOPE  2
#define APP_MODE_BOTH         3

int appMode = APP_MODE_BOTH;   // default autonomous mode

#define STETH_INPUT_LINE 1

// ===================== PIN MAP =====================
static const int MY_I2C_SDA   = 3;
static const int MY_I2C_SCL   = 4;

static const int MY_BUTTON    = 12;   // active LOW

static const int MY_MPRLS_EOC = 10;   // Feather D10
static const int MY_MPRLS_RST = 11;   // Feather D11

static const int MY_I2S_MCLK  = 14;
static const int MY_I2S_BCLK  = 36;
static const int MY_I2S_LRCLK = 8;
static const int MY_I2S_DOUT  = 35;
static const int MY_I2S_DIN   = 37;

// ===================== OLED =====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET   -1
#define OLED_ADDR    0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===================== PRESSURE SENSOR =====================
Adafruit_MPRLS mpr(
  MY_MPRLS_RST,
  MY_MPRLS_EOC,
  0.0f,
  300.0f,
  2.5f,
  22.5f,
  1.0f
);

float pressureZero = 0.0f;
bool pressureInitialized = false;
unsigned long awakeStartMs = 0;
bool autoShutoffTriggered = false;

// ===================== ES8388 / I2S =====================
#define I2S_PORT I2S_NUM_0
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_DMA_BUF_LEN 128
#define AUDIO_DMA_BUF_COUNT 8
#define AUDIO_BLOCK_FRAMES 256

int16_t audioBuffer[AUDIO_BLOCK_FRAMES * 2];
bool audioInitialized = false;
int outputVolume = 80;
bool plotAudioEnabled = false;
int plotDecimation = 8;
Adafruit_MAX17048 maxlipo;
bool batteryGaugeInitialized = false;

audio_driver::DriverPins codecPins;
AudioBoard audioBoard(AudioDriverES8388, codecPins);

// ===================== TIMING =====================
unsigned long lastPressureMs = 0;
unsigned long lastOledMs = 0;

// ===================== HELPERS =====================
bool initBatteryGauge() {
  if (!maxlipo.begin()) {
    Serial.println("MAX17048 init failed");
    return false;
  }
  Serial.println("MAX17048 battery gauge ready");
  return true;
}

void drawBatteryPercentTopRight() {
  if (!batteryGaugeInitialized) return;

  float percent = maxlipo.cellPercent();
  if (isnan(percent)) return;

  if (percent < 0.0f) percent = 0.0f;
  if (percent > 100.0f) percent = 100.0f;

  char buf[8];
  snprintf(buf, sizeof(buf), "%d%%", (int)(percent + 0.5f));

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(SCREEN_WIDTH - w+15, 0);
  display.print(buf);
}

void oledMessage(const String &line1,
                 const String &line2 = "",
                 const String &line3 = "",
                 bool showBattery = true) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(line1);
  if (line2.length()) display.println(line2);
  if (line3.length()) display.println(line3);

  if (showBattery) {
    drawBatteryPercentTopRight();
  }

  display.display();
}

void printModeHelp() {
  Serial.println();
  Serial.println("Runtime commands:");
  Serial.println("  p = Pressure only");
  Serial.println("  s = Stethoscope only");
  Serial.println("  b = Both (pressure on OLED + audio active)");
  Serial.println("  t = toggle audio plot on/off");
  Serial.println("  y = print plot status");
  Serial.println("  h = Help");
  Serial.println("  < = output volume down");
  Serial.println("  > = output volume up");
  Serial.println("  v = print output volume");
  Serial.println();
}

bool buttonPressed() {
  return digitalRead(MY_BUTTON) == LOW;
}

void waitForButtonRelease() {
  while (digitalRead(MY_BUTTON) == LOW) {
    delay(10);
  }
  delay(50);
}

void enterDeepSleep() {
  oledMessage("Sleeping...", "Release button", "Press to wake", false);
  delay(5000);

  display.clearDisplay();
  display.display();

  waitForButtonRelease();

  rtc_gpio_init(GPIO_NUM_12);
  rtc_gpio_set_direction(GPIO_NUM_12, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en(GPIO_NUM_12);
  rtc_gpio_pulldown_dis(GPIO_NUM_12);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
  delay(50);
  esp_deep_sleep_start();
}

void handleSleepButton() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(MY_BUTTON);

  if (lastState == HIGH && currentState == LOW) {
    delay(30);
    if (digitalRead(MY_BUTTON) == LOW) {
      enterDeepSleep();
    }
  }

  lastState = currentState;
}

void printWakeReason() {
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  Serial.print("Wake reason: ");
  switch (cause) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("EXT0 (button)");
      break;
    default:
      Serial.println("power-on/reset/other");
      break;
  }
}

bool initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    return false;
  }

  display.setRotation(2);
  display.clearDisplay();
  display.display();
  return true;
}

bool initPressure() {
  if (!mpr.begin()) {
    Serial.println("MPRLS init failed");
    return false;
  }
  return true;
}

float measurePressureZero(int samples = 25) {
  float sum = 0.0f;
  int count = 0;

  for (int i = 0; i < samples; i++) {
    float p = mpr.readPressure();
    if (!isnan(p)) {
      sum += p;
      count++;
    }
    delay(20);
  }

  if (count == 0) return 0.0f;
  return sum / count;
}

bool initES8388ForStethoscope() {
  Wire.begin(MY_I2C_SDA, MY_I2C_SCL);

  codecPins.addI2C(
    audio_driver::PinFunction::CODEC,
    MY_I2C_SCL,
    MY_I2C_SDA,
    0x10,
    100000,
    Wire
  );

  codecPins.addI2S(
    audio_driver::PinFunction::CODEC,
    MY_I2S_MCLK,
    MY_I2S_BCLK,
    MY_I2S_LRCLK,
    MY_I2S_DIN,
    MY_I2S_DOUT
  );

  audio_driver::CodecConfig cfg;
  cfg.output_device = DAC_OUTPUT_LINE1;
#if STETH_INPUT_LINE == 1
  cfg.input_device  = ADC_INPUT_LINE1;
#else
  cfg.input_device  = ADC_INPUT_LINE2;
#endif
  cfg.i2s.bits      = BIT_LENGTH_16BITS;
  cfg.i2s.rate      = RATE_16K;
  cfg.i2s.channels  = CHANNELS2;
  cfg.i2s.fmt       = I2S_NORMAL;
  cfg.i2s.mode      = MODE_SLAVE;

  if (!audioBoard.begin(cfg)) {
    Serial.println("ES8388 init failed");
    return false;
  }

  audioBoard.setVolume(outputVolume);
  audioBoard.setInputVolume(50);

  Serial.println("ES8388 configured for stethoscope");
  return true;
}

bool initLowLevelI2SPassthrough() {
  i2s_config_t i2s_config = {};
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX);
  i2s_config.sample_rate = AUDIO_SAMPLE_RATE;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2s_config.intr_alloc_flags = 0;
  i2s_config.dma_buf_count = AUDIO_DMA_BUF_COUNT;
  i2s_config.dma_buf_len = AUDIO_DMA_BUF_LEN;
  i2s_config.use_apll = true;
  i2s_config.tx_desc_auto_clear = true;
  i2s_config.fixed_mclk = AUDIO_SAMPLE_RATE * 256;

  i2s_pin_config_t pin_config = {};
  pin_config.mck_io_num = MY_I2S_MCLK;
  pin_config.bck_io_num = MY_I2S_BCLK;
  pin_config.ws_io_num = MY_I2S_LRCLK;
  pin_config.data_out_num = MY_I2S_DOUT;
  pin_config.data_in_num  = MY_I2S_DIN;

  esp_err_t err;

  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("i2s_driver_install failed: %d\n", err);
    return false;
  }

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("i2s_set_pin failed: %d\n", err);
    return false;
  }

  err = i2s_set_clk(I2S_PORT, AUDIO_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
  if (err != ESP_OK) {
    Serial.printf("i2s_set_clk failed: %d\n", err);
    return false;
  }

  return true;
}

bool ensurePressureReady() {
  if (pressureInitialized) return true;

  oledMessage("Pressure mode", "Initializing...");
  if (!initPressure()) {
    oledMessage("Pressure init", "FAILED");
    return false;
  }

  oledMessage("Zeroing...", "Open cuff to air");
  delay(500);
  pressureZero = measurePressureZero();
  pressureInitialized = true;

  Serial.println("Pressure subsystem ready");
  return true;
}

bool ensureAudioReady() {
  if (audioInitialized) return true;

  oledMessage("Stethoscope", "Config codec...");
  if (!initES8388ForStethoscope()) {
    oledMessage("Codec init", "FAILED");
    return false;
  }

  oledMessage("Stethoscope", "Config I2S...");
  if (!initLowLevelI2SPassthrough()) {
    oledMessage("I2S init", "FAILED");
    return false;
  }

  audioInitialized = true;
  Serial.println("Audio subsystem ready");
  return true;
}

void showModeBanner() {
  if (appMode == APP_MODE_PRESSURE) {
    oledMessage("Pressure mode", "Ready", "Btn -> Sleep");
  } else if (appMode == APP_MODE_STETHOSCOPE) {
#if STETH_INPUT_LINE == 1
    oledMessage("Recording...", "ADC L1 -> OUT L1", "Btn -> Sleep");
#else
    oledMessage("Recording...", "ADC L2 -> OUT L1", "Btn -> Sleep");
#endif
  } else {
    oledMessage("Both mode", "Pressure + Audio", "Btn -> Sleep");
  }
}

void setAppMode(int newMode) {
  if (newMode == appMode) return;

  if ((newMode == APP_MODE_PRESSURE || newMode == APP_MODE_BOTH) && !ensurePressureReady()) {
    Serial.println("Could not switch mode: pressure init failed");
    return;
  }

  if ((newMode == APP_MODE_STETHOSCOPE || newMode == APP_MODE_BOTH) && !ensureAudioReady()) {
    Serial.println("Could not switch mode: audio init failed");
    return;
  }

  appMode = newMode;
  lastOledMs = 0;
  lastPressureMs = 0;

  Serial.print("Mode changed to: ");
  if (appMode == APP_MODE_PRESSURE) Serial.println("Pressure");
  else if (appMode == APP_MODE_STETHOSCOPE) Serial.println("Stethoscope");
  else Serial.println("Both");

  showModeBanner();
}

void handleSerialCommands() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n' || c == '\r' || c == ' ') {
      continue;
    }

    if (c == 'p' || c == 'P') {
      setAppMode(APP_MODE_PRESSURE);
    } else if (c == 's' || c == 'S') {
      setAppMode(APP_MODE_STETHOSCOPE);
    } else if (c == 'b' || c == 'B') {
      setAppMode(APP_MODE_BOTH);
    } else if (c == 'h' || c == 'H' || c == '?') {
      printModeHelp();
    } else if (c == '>') {
      outputVolume += 5;
      if (outputVolume > 100) outputVolume = 100;
      if (audioInitialized) audioBoard.setVolume(outputVolume);
      Serial.print("Output volume = ");
      Serial.println(outputVolume);
    } else if (c == '<') {
      outputVolume -= 5;
      if (outputVolume < 0) outputVolume = 0;
      if (audioInitialized) audioBoard.setVolume(outputVolume);
      Serial.print("Output volume = ");
      Serial.println(outputVolume);
    } else if (c == 'v' || c == 'V') {
      Serial.print("Output volume = ");
      Serial.println(outputVolume);
    } else if (c == 't' || c == 'T') {
      plotAudioEnabled = !plotAudioEnabled;
      Serial.print("Audio plot = ");
      Serial.println(plotAudioEnabled ? "ON" : "OFF");
    } else if (c == 'y' || c == 'Y') {
      Serial.print("Audio plot = ");
      Serial.println(plotAudioEnabled ? "ON" : "OFF");
    } else {
      Serial.print("Unknown command: ");
      Serial.println(c);
      printModeHelp();
    }
  }
}

void enterAutoShutoffDeepSleep() {
  autoShutoffTriggered = true;

  oledMessage("AUTO SHUTOFF", "", "", false);
  delay(1200);   // brief message for the presentation

  display.clearDisplay();
  display.display();

  waitForButtonRelease();

  rtc_gpio_init(GPIO_NUM_12);
  rtc_gpio_set_direction(GPIO_NUM_12, RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en(GPIO_NUM_12);
  rtc_gpio_pulldown_dis(GPIO_NUM_12);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 0);
  delay(50);
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  printWakeReason();

  pinMode(MY_BUTTON, INPUT_PULLUP);
  Wire.begin(MY_I2C_SDA, MY_I2C_SCL);

  if (!initOLED()) {
    while (true) delay(100);
  }

  batteryGaugeInitialized = initBatteryGauge();

  printModeHelp();
  Serial.println("Booting immediately in default mode: Both");

  if (!ensurePressureReady()) {
    while (true) {
      handleSleepButton();
      delay(20);
    }
  }

  if (!ensureAudioReady()) {
    while (true) {
      handleSleepButton();
      delay(20);
    }
  }

  appMode = APP_MODE_BOTH;
  showModeBanner();
  awakeStartMs = millis();
autoShutoffTriggered = false;
}

void loop() {
  handleSleepButton();
  handleSerialCommands();

  if (!autoShutoffTriggered && millis() - awakeStartMs >= 16200000UL) {
  enterAutoShutoffDeepSleep();
}

  if ((appMode == APP_MODE_STETHOSCOPE || appMode == APP_MODE_BOTH) && audioInitialized) {
    size_t bytesRead = 0;
    size_t bytesWritten = 0;

    esp_err_t r = i2s_read(I2S_PORT, audioBuffer, sizeof(audioBuffer), &bytesRead, portMAX_DELAY);

    if (r == ESP_OK && bytesRead > 0) {
      int sampleCount = bytesRead / sizeof(int16_t);

      static int32_t prevIn = 0;
      static int32_t dcBlockY = 0;
      static int32_t lp1 = 0;
      static int32_t lp2 = 0;

      for (int i = 0; i < sampleCount; i += 2) {
        int32_t right = audioBuffer[i + 1];

        int32_t mono = right;

        int32_t hp = mono - prevIn + ((dcBlockY * 992) / 1000);
        prevIn = mono;
        dcBlockY = hp;

        lp1 = lp1 + ((hp  - lp1) / 10);
        lp2 = lp2 + ((lp1 - lp2) / 10);

        static int plotCounter = 0;
        if (plotAudioEnabled) {
          plotCounter++;
          if (plotCounter >= plotDecimation) {
            plotCounter = 0;
            Serial.println(lp2);
          }
        }

        int32_t y = lp2;

        if (y > 32767) y = 32767;
        if (y < -32768) y = -32768;

        audioBuffer[i]     = (int16_t)y;
        audioBuffer[i + 1] = (int16_t)y;
      }

      i2s_write(I2S_PORT, audioBuffer, bytesRead, &bytesWritten, portMAX_DELAY);
    }
  }

  if ((appMode == APP_MODE_PRESSURE || appMode == APP_MODE_BOTH) &&
      pressureInitialized &&
      millis() - lastPressureMs >= 150) {
    lastPressureMs = millis();

    float pressure_mmHg = (mpr.readPressure()-pressureZero)/0.9762f;
    if (isnan(pressure_mmHg)) {
      oledMessage("Pressure error", "Read failed");
      return;
    }

    if (pressure_mmHg < 0.0f) pressure_mmHg = 0.0f;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    if (appMode == APP_MODE_BOTH) {
      display.println("Pressure + Audio");
    } else {
      display.println("Pressure");
    }

    display.setTextSize(2);
    display.setCursor(0, 14);
    display.print(pressure_mmHg, 1);
    display.print(" mmHg");
    drawBatteryPercentTopRight();
    display.display();
  }

  if (appMode == APP_MODE_STETHOSCOPE &&
      millis() - lastOledMs >= 500) {
    lastOledMs = millis();

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Recording...");
#if STETH_INPUT_LINE == 1
    display.println("ADC L1 -> OUT L1");
#else
    display.println("ADC L2 -> OUT L1");
#endif
    display.println("Btn -> Sleep");
    drawBatteryPercentTopRight();
    display.display();
  }
}
