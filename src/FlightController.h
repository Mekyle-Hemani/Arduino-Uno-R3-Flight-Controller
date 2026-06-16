#ifndef FlightController_h
#define FlightController_h

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <U8x8lib.h>
#include <SD.h>

#define SD_CS_PIN 10

class FlightController {
    public:
    FlightController();
    
    //All the functions that check and start components of the project
    bool begin();
    bool beginMPU();
    bool beginOLED();
    bool beginSDCard();

    //MPU-6050
    void zeroControllerAxis(); //Makes the current position of the controller the "zero"
    void getRotationRaw(float &x, float &y, float &z); //Gets the current rotation of the pitch, yaw and roll at once
    
    //Gets all axis rotations individually
    float getPitch();
    float getYaw();
    float getRoll();

    float getTemperature(); //Gets the current on-board temperature

    //Basic activation commands to check the motion of the board
    bool isMoving(float threshold = 0.05);
    bool isFlatLevel(float tolerance = 10.0);

    //OLED
    void writeText(const char* text, int x, int y, int size = 1); //Writes text to the screen
    void clearDisplay(); //Clears all screen text

    //LED
    void blinkLED(int delayMs = 500); //Blinks the LED

    //SD Card Reader
    String readSDCardTextFile(); //Returns the first line of text.txt


  private:
  float _offsetPitch;
  float _offsetRoll;
  float _offsetYaw;
  bool _sdReady;
  Adafruit_MPU6050 _mpu;
};

#endif