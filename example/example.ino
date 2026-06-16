#include <FlightController.h>

FlightController fc;

int mode;

void setup() {
  //Start by blinking the LED quickly 3 times to show power
  for (int i = 0; i<3; i++) fc.blinkLED(150);
  //Add a delay after the Arduino boots to ensure that there are no startup-delay based errors when checking the I2C bus
  delay(1000);
  Serial.begin(9600);

  //Wait until the serial port opens
  while (!Serial) {;}
  //Tell the user that we are now going to check the addresses of all required components through the serial port
  Serial.println(F("Booting Arduino Flight Controller"));
  delay(500);

  //If there is an error regarding starting up the components, read which components failed and flash the error light
  Serial.print(F("Initializing I2C Bus... "));
  if (fc.begin() == false) {
    Serial.println(F("Hardware Allocation Failed"));
    while (true) {
      fc.blinkLED();
    }
  }
  //Claim that all components were found properly
  Serial.println(F("Passed"));

  {
    String raw = fc.readSDCardTextFile();
    if (raw.length() > 0 && raw[0] >= '0' && raw[0] <= '2') {
      mode = raw[0] - '0';
    } else {
      Serial.println(F("Bad/missing mode on SD"));
      mode = 3;
    }
  }

  //Print the found mode
  Serial.print(F("Mode: "));
  Serial.println(mode);

  //Calibrate the MPU-6050 Gyro
  Serial.print(F("Calibrating Gyro... "));
  delay(500);
  fc.zeroControllerAxis();
  Serial.println(F("Passed"));

  //Start entering the data loop determined by the SD Card
  Serial.println(F("Entering Data Loop"));
}

//Picks the mode that the SD Card had on its text.txt file and run the code on that mode
void loop() {
  if (mode == 0) {
    mode0();
  } else if (mode == 1) {
    mode1();
  } else if (mode == 2) {
    mode2(); 
  } else {
    //If the SD Card isn't present etc, flash the error light
    Serial.println(F("Mode not flashed correctly"));
    while (true) {
      fc.blinkLED();
    }
  }
}

//Mode0 will print the pitch, roll, yaw and temperature over time
void mode0() {
  Serial.print(F("Pitch: "));
  Serial.println(fc.getPitch());

  Serial.print(F("Roll: "));
  Serial.println(fc.getRoll());

  Serial.print(F("Yaw: "));
  Serial.println(fc.getYaw());

  Serial.print(F("Temperature: "));
  Serial.print(fc.getTemperature());
  Serial.println(F(" C"));
  Serial.println(); 

  delay(1000);
}

//Mode1 will print if the Arduino is moving over time and if it is levelled
void mode1() {
  Serial.print(F("Is moving?: "));
  Serial.println(fc.isMoving());

  Serial.print(F("Is flat level??: "));
  Serial.println(fc.isFlatLevel());
  delay(500); 
}

//Mode0 will print the pitch, roll, yaw and temperature on the OLED over time
void mode2() {
  float pitch = fc.getPitch();
  float roll = fc.getRoll();
  float yaw = fc.getYaw();
  float temp = fc.getTemperature();

  char pStr[7], rStr[7], yStr[7], tStr[7];
  char buf[22]; 

  dtostrf(pitch, 4, 1, pStr);
  dtostrf(roll, 4, 1, rStr);
  dtostrf(yaw, 4, 1, yStr);
  dtostrf(temp, 4, 1, tStr);

  fc.clearDisplay();

  snprintf(buf, sizeof(buf), "P:%s R:%s", pStr, rStr);
  fc.writeText(buf, 0, 0, 1);
  Serial.println(buf);

  snprintf(buf, sizeof(buf), "Y:%s", yStr);
  fc.writeText(buf, 0, 16, 1);
  Serial.println(buf);

  snprintf(buf, sizeof(buf), "Temp:%sC", tStr);
  fc.writeText(buf, 0, 32, 1);
  Serial.println(buf);
  
  delay(500);
}