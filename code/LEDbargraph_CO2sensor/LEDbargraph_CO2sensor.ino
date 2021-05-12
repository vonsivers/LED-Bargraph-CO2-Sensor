#include "esp_timer.h"
#include <Wire.h>

// Download the SeeedStudio SCD30 Arduino driver here:
//  => https://github.com/Seeed-Studio/Seeed_SCD30/releases/latest
#include "SCD30.h"

#include "Sensirion_GadgetBle_Lib.h"

#define SDA_pin 21  // Define the SDA pin used for the SCD30
#define SCL_pin 22  // Define the SCL pin used for the SCD30

// pins connected to LED bargraph
#define LED1 15
#define LED2 13
#define LED3 12
#define LED4 14
#define LED5 27
#define LED6 26
#define LED7 25
#define LED8 33
#define LED9 32
#define LED10 23

static int64_t lastMmntTime = 0;
static int startCheckingAfterUs = 1900000;

GadgetBle gadgetBle = GadgetBle(GadgetBle::DataType::T_RH_CO2);

void allLEDsOff() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);
  digitalWrite(LED6, LOW);
  digitalWrite(LED7, LOW);
  digitalWrite(LED8, LOW);
  digitalWrite(LED9, LOW);
  digitalWrite(LED10, LOW);
}

void allLEDsOn() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);
  digitalWrite(LED6, HIGH);
  digitalWrite(LED7, HIGH);
  digitalWrite(LED8, HIGH);
  digitalWrite(LED9, HIGH);
  digitalWrite(LED10, HIGH);
}

void lightBarGraph(uint8_t N) {
  switch(N) {
    case 1:
      digitalWrite(LED1, HIGH);
      break;
    case 2:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      break;
    case 3:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      break;
    case 4:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      break;
    case 5:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      break;
    case 6:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      break;
    case 7:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      digitalWrite(LED7, HIGH);
      break;
    case 8:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      digitalWrite(LED7, HIGH);
      digitalWrite(LED8, HIGH);
      break;
    case 9:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      digitalWrite(LED6, HIGH);
      digitalWrite(LED7, HIGH);
      digitalWrite(LED8, HIGH);
      digitalWrite(LED9, HIGH);
      break;
    case 10:  // light and blink all LEDs
      allLEDsOn();
      delay(200);
      allLEDsOff();
      delay(200);
      allLEDsOn();
      delay(200);
      allLEDsOff();
      delay(200);
      allLEDsOn();
      break;
  }

}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  pinMode(LED7, OUTPUT);
  pinMode(LED8, OUTPUT);
  pinMode(LED9, OUTPUT);
  pinMode(LED10, OUTPUT);

  allLEDsOff();
  
  Serial.begin(115200);
  delay(100);


  // Initialize the GadgetBle Library
  gadgetBle.begin();
  Serial.print("Sensirion GadgetBle Lib initialized with deviceId = ");
  Serial.println(gadgetBle.getDeviceIdString());

  // Initialize the SCD30 driver
  Wire.begin(SDA_pin, SCL_pin);
  scd30.initialize();
  scd30.setAutoSelfCalibration(1);
  scd30.setTemperatureOffset(3);
}

void loop() {
  float result[3] = {0};

  if (esp_timer_get_time() - lastMmntTime >= startCheckingAfterUs) {

    if (scd30.isAvailable()) {
      scd30.getCarbonDioxideConcentration(result);

      gadgetBle.writeCO2(result[0]);
      gadgetBle.writeTemperature(result[1]);
      gadgetBle.writeHumidity(result[2]);

      gadgetBle.commit();
      lastMmntTime = esp_timer_get_time();

      // show CO2 concentration on bargraph
      uint8_t N;
      if(result[0]<1000) {    // 400-1000ppm: green LED (1-5)
        N = constrain(map(result[0],400,1000,1,5),1,5);
      }
      else if(result[0]<1600) { // 1000-1600ppm: yellow LED (6-8)
        N = constrain(map(result[0],1000,1600,6,8),6,8);
      }
      else {  // >1600ppm: red LED (9-10)
        N = constrain(map(result[0],1600,5000,9,10),9,10);
      }
      allLEDsOff();
      lightBarGraph(N);
      

      // Provide the sensor values for Tools -> Serial Monitor or Serial Plotter
      Serial.print("CO2[ppm]:");
      Serial.print(result[0]);
      Serial.print("\t");
      Serial.print("Temperature[℃]:");
      Serial.print(result[1]);
      Serial.print("\t");
      Serial.print("Humidity[%]:");
      Serial.println(result[2]);

      Serial.print("LED bargraph value: "); Serial.println(N);
    }
  }

  gadgetBle.handleEvents();
  delay(3);
}
