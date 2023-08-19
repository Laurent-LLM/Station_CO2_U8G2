#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h"
SCD30 scd30;

#include <U8x8lib.h>
U8X8_SH1106_128X64_NONAME_HW_I2C screen(/* reset=*/U8X8_PIN_NONE);

#include <Adafruit_NeoPixel.h>

#define LED_PIN D6
#define NUM_LEDS 8
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t vert = strip.Color(0, 255, 0);
uint32_t jaune = strip.Color(255, 255, 0);
uint32_t orange = strip.Color(255, 128, 0);
uint32_t rouge = strip.Color(255, 0, 0);
uint32_t blanc = strip.Color(255, 255, 255);

char buf[6];

void setup() {

  Serial.begin(115200);

  Wire.begin();
  // Wire.setClock(10000);

  screen.begin();
  screen.setFont(u8x8_font_amstrad_cpc_extended_f);

 delay(10);
 
  if (!scd30.begin()) {
    Serial.println("Echec de l'initialisation du capteur SCD30 !");
  } else {
    Serial.println("Capteur SCD30 initialise !");
  }

  strip.begin();
  strip.setBrightness(20);
  strip.show();  // Éteindre toutes les LEDs
}

void loop() {

  if (scd30.dataAvailable()) {
    if (scd30.readMeasurement()) {
      float co2 = scd30.getCO2();
      float temperature = scd30.getTemperature();
      float humidity = scd30.getHumidity();

      // Faire quelque chose avec les valeurs de CO2, température et humidité
      Serial.print("CO2 (ppm): ");
      Serial.println(co2);
      Serial.print("Température (°C): ");
      Serial.println(temperature);
      Serial.print("Humidité (%): ");
      Serial.println(humidity);

      // Mettre à jour l'écran OLED
      updateOLED(co2, temperature, humidity);

      // Mettre à jour la barre de LEDs
      updateLEDs(co2);
    }
  }

  delay(1000);
}

void updateOLED(float co2, float temperature, float humidity) {
  //screen.clearDisplay();

  String airQuality = getAirQualityText(co2);
  screen.draw1x2UTF8(0, 0, airQuality.c_str());

  screen.drawString(0, 3, "Temp.:");
  dtostrf(temperature, 5, 2, buf);
  screen.drawString(7, 3, buf);
  screen.drawUTF8(12, 3, " °C");

  screen.drawString(0, 4, "Hum. :");
  dtostrf(humidity, 5, 0, buf);
  screen.drawString(7, 4, buf);
  screen.drawString(12, 4, " %");

  screen.drawString(0, 5, "CO2  :");
  dtostrf(co2, 5, 0, buf);
  screen.drawString(7, 5, buf);
  screen.drawString(12, 5, " ppm");
}

void updateLEDs(float co2) {
  switch (getCO2Level(co2)) {
    case 1:
      strip.fill(vert, 0, 8);
      break;
    case 2:
      strip.fill(vert, 0, 4);
      strip.fill(jaune, 4, 4);
      break;
    case 3:
      strip.fill(jaune, 0, 8);
      break;
    case 4:
      strip.fill(jaune, 0, 4);
      strip.fill(orange, 4, 4);
      break;
    case 5:
      strip.fill(orange, 0, 8);
      break;
    case 6:
      strip.fill(orange, 0, 4);
      strip.fill(rouge, 4, 4);
      break;
    case 7:
      strip.fill(rouge, 0, 8);
      break;
    case 8:
      strip.fill(blanc, 0, 8);
      break;
    default:
      break;
  };

  strip.show();
}

int getCO2Level(float co2) {
  if (co2 < 800) {
    return 1;  // Niveau 1 (très bon)
  } else if (co2 < 1000) {
    return 2;  // Niveau 2 (bon)
  } else if (co2 < 1200) {
    return 3;  // Niveau 3 (acceptable)
  } else if (co2 < 1400) {
    return 4;  // Niveau 4 (médiocre)
  } else if (co2 < 1600) {
    return 5;  // Niveau 5 (mauvais)
  } else if (co2 < 1800) {
    return 6;  // Niveau 6 (très mauvais)
  } else if (co2 < 2000) {
    return 7;  // Niveau 7 (dangereux)
  } else {
    return 8;  // Niveau 8 (extrêmement dangereux)
  }
}

String getAirQualityText(float co2) {
  int level = getCO2Level(co2);

  switch (level) {
    case 1:
      return "    Très bon   ";
    case 2:
      return "      Bon      ";
    case 3:
      return "   Acceptable  ";
    case 4:
      return "    Médiocre   ";
    case 5:
      return "    Mauvais    ";
    case 6:
      return "  Très mauvais ";
    case 7:
      return "   Dangereux   ";
    case 8:
      return "Très dangereux ";
    default:
      return "Inconnu";
  }
}
