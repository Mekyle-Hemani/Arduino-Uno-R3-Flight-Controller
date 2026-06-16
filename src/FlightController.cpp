#include "FlightController.h"
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

FlightController::FlightController() {
    _offsetPitch = 0;
    _offsetRoll = 0;
    _offsetYaw = 0;
    pinMode(8, OUTPUT);
}

//The process that checks all components of the flight controller
bool FlightController::begin() {
    Wire.begin();
    delay(100);
    if (!beginOLED()) {
        Serial.println("OLED failed");
        return false;
    }
    if (!beginMPU()) {
        Serial.println("MPU-6050 failed");
        return false;
    }
    if (!beginSDCard()) {
        Serial.println("SD Card failed");
    }
    return true; //Only return true if the SD Card, MPU-6050 and OLED have I2C Bus Address
}

//Checks for the address of the MPU-6050
bool FlightController::beginMPU() {
    if (!_mpu.begin()) {
        return false;
    }
    return true;
}

//Checks for the address of the OLED
bool FlightController::beginOLED() {
    delay(250);
    if (!u8x8.begin()) {
        return false;
    }
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.clearDisplay();
    return true;
}

//Checks for the address of the SD Card Adapter
bool FlightController::beginSDCard() {
    if (!SD.begin(SD_CS_PIN)) {
        _sdReady = false;
        return false;
    }
    _sdReady = true;
    return true;
}

//MPU-6050 functions
void FlightController::zeroControllerAxis() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    //Set the new offsets to the rotations so that the current rotation subtracted by the offset is 0
    _offsetPitch = g.gyro.x;
    _offsetRoll = g.gyro.y;
    _offsetYaw = g.gyro.z;
}

void FlightController::getRotationRaw(float &x, float &y, float &z) {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    //Gets the current position subtracted by the offset and sets those to current variables for reference later
    x = g.gyro.x - _offsetPitch;
    y = g.gyro.y - _offsetRoll;
    z = g.gyro.z - _offsetYaw;
}

float FlightController::getPitch() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    return g.gyro.x - _offsetPitch; //Returns the current pitch subtracted by the offset pitch
}

float FlightController::getYaw() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    return g.gyro.z - _offsetYaw; //Returns the current yaw subtracted by the offset yaw
}

float FlightController::getRoll() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    return g.gyro.y - _offsetRoll; //Returns the current roll subtracted by the offset roll
}

float FlightController::getTemperature() {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp); //Get all the available sensor details
    return temp.temperature; //Returns the raw temperature data
}

//Checks if the rotation in the x, y and z are beyond the range of threshold, then return if it is or not
bool FlightController::isMoving(float threshold) {
    float x, y, z;
    getRotationRaw(x, y, z);
    if (abs(x) > threshold) {
        return true;
    }
    if (abs(y) > threshold) {
        return true;
    }
    if (abs(z) > threshold) {
        return true;
    }
    return false;
}

//Runs pitch and roll calculations to see if the MPU-6050 is levelled or not
bool FlightController::isFlatLevel(float tolerance) {
    sensors_event_t a, g, temp;
    _mpu.getEvent(&a, &g, &temp);
    float pitch = atan2(a.acceleration.x, a.acceleration.z) * 180.0 / PI;
    float roll = atan2(a.acceleration.y, a.acceleration.z) * 180.0 / PI;
    return (abs(pitch) < tolerance && abs(roll) < tolerance);
}


//OLED functions
void FlightController::writeText(const char* text, int x, int y, int size) {
    int col = x / 8;
    int row = y / 8;
    u8x8.drawString(col, row, text);
}

void FlightController::clearDisplay() {
    u8x8.clearDisplay();
}

//LED
void FlightController::blinkLED(int delayMs) {
    digitalWrite(8, HIGH);
    delay(delayMs);
    digitalWrite(8, LOW);
    delay(delayMs);
}

//SD Card Reader
String FlightController::readSDCardTextFile() {
    //Initialise the SD Card on CS pin 10
    //Reads the very first line of text.txt on the SD Card
    if (!_sdReady) {
        Serial.println("SD not initialised"); return "";
    }

    File f = SD.open("text.txt", FILE_READ);
    if (!f) {
        Serial.println("SD Card Could not open text.txt");
        return "";
    }

    String line = "";
    while (f.available()) {
        char c = f.read();
        if (c == '\n' || c == '\r') break;
        line += c;
    }

    f.close();
    return line;
}