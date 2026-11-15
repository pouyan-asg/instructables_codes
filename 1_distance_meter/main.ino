#include <Wire.h>                 // Adds I²C communication for the Qwiic sensor.
#include <LiquidCrystal.h>        // Adds commands for the 16-pin LCD.
#include "SparkFun_VL53L1X.h"     // Adds SparkFun's distance sensor commands.

const uint8_t GREEN_LED_PIN = 4;   // Green LED uses RedBoard digital pin D4.
const uint8_t YELLOW_LED_PIN = 5;  // Yellow LED uses D5.
const uint8_t RED_LED_PIN = 6;     // Red LED uses D6.
// const means these values stay fixed. uint8_t is an 8-bit unsigned integer.

// Creates the LCD object. The six arguments are RedBoard pins for
// these LCD signals in order: RS, E, DB4, DB5, DB6, DB7.
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

SFEVL53L1X distanceSensor;         // Creates the object used to control the sensor.

void setup()                     // Runs once. void means it returns no value.
{
  pinMode(GREEN_LED_PIN, OUTPUT);  // Makes D4 an output so it can drive the LED.
  pinMode(YELLOW_LED_PIN, OUTPUT); // Makes D5 an output.
  pinMode(RED_LED_PIN, OUTPUT);    // Makes D6 an output.

  digitalWrite(GREEN_LED_PIN, LOW);  // Starts with green off.
  digitalWrite(YELLOW_LED_PIN, LOW); // Starts with yellow off.
  digitalWrite(RED_LED_PIN, LOW);    // Starts with red off.

  lcd.begin(16, 2);                // Configures the LCD for 16 columns and 2 rows.
  lcd.print("Distance");          // Writes the heading on the top row.

  Wire.begin();                   // Starts I²C communication.
  Serial.begin(115200);           // Sets the Serial Monitor speed to 115200 baud.
  distanceSensor.setOffset(2037);  // Keeps the offset setting from your original sketch.

  if (distanceSensor.begin() != 0) // Tries to start the sensor. A nonzero result means failure.
  {
    lcd.setCursor(0, 1);           // Goes to the first column of the second row.
    lcd.print("Sensor error");     // Shows the error on the LCD.
    Serial.println("Sensor failed to begin. Please check wiring.");
                                   // Also shows the error in the Serial Monitor.
    while (true)                   // Stays here indefinitely if startup failed.
    {
      delay(1000);                 // Waits one second on each pass through this block.
    }
  }

  Serial.println("VL53L4CD sensor is online!"); // Reports successful startup.
}

void loop()                       // Runs repeatedly after successful setup.
{
  distanceSensor.startRanging();   // Starts a distance measurement.

  while (!distanceSensor.checkForDataReady()) // The ! means "while data is not ready."
  {
    delay(1);                      // Waits 1 ms before checking again.
  }

  // Stores the reading in millimeters as a 16-bit unsigned integer.
  const uint16_t distanceMm = distanceSensor.getDistance();

  distanceSensor.clearInterrupt(); // Clears the sensor's measurement-ready flag.
  distanceSensor.stopRanging();    // Stops ranging until the next pass through loop().

  // condition ? HIGH : LOW selects HIGH (LED on) when the condition is true.
  digitalWrite(GREEN_LED_PIN, distanceMm < 50 ? HIGH : LOW);                     // Below 50 mm.
  digitalWrite(YELLOW_LED_PIN, distanceMm >= 50 && distanceMm <= 120 ? HIGH : LOW); // 50–120 mm, inclusive.
  digitalWrite(RED_LED_PIN, distanceMm > 120 ? HIGH : LOW);                      // Above 120 mm.
  // && means both comparisons must be true.

  lcd.setCursor(0, 1);              // Goes back to the start of the second row.
  lcd.print(distanceMm);            // Writes the new distance.
  lcd.print(" mm     ");            // Adds the unit and overwrites leftover characters.

  Serial.print("Distance(mm): ");   // Writes the label without starting a new line.
  Serial.println(distanceMm);       // Writes the reading and ends the line.

  delay(100);                       // Waits 100 ms before starting the next measurement.
}