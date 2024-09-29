#include <Wire.h>
#include <MPU6050_light.h>

#define buadRate 9600

// String constants used for parsing serial commands
String sequenceCommand = "executeSequence"  , pathCommand = "executePath";
String path_angleCommand = "angle"          , path_angleDirCommand = "dir";
String path_distanceCommand = "distance"    , path_speedCommand = "speed";
String path_substringCommand = "&"          , path_equalCommand = "=";

int pathDetails[3]; // Array to store path parameters (angle, distance, speed)

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

// Motor control pins
#define motorL1   7
#define motorL2   6
#define motorR1   9
#define motorR2   8
#define motorEN1  5
#define motorEN2  10
#define sensor    2

// Speed settings
#define speedTurn           80
#define speed_error_factor  11
#define speed_adjust_factor 10


// Shape dimensions and speeds
#define squareLength        100
#define squareSpeed         150

#define triangleLength      100
#define triangleSpeed       150

#define rectangleLength     100
#define rectangleWidth      150
#define rectangleSpeed      150

int steps = 0;  // Tracks the number of steps (encoder counts)
int distance = 0;  // Distance traveled based on encoder steps

void setup() {
  Serial.begin(buadRate); // Initialize serial communication at a baud rate of 9600
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  calculate_IMU_error();
  update_mpu_readings();
  pinMode(sensor, INPUT); // Set sensor pin as input
  pinMode(motorR1, OUTPUT); // Set motor control pins as outputs
  pinMode(motorR2, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  pinMode(motorEN1, OUTPUT);
  pinMode(motorEN2, OUTPUT);
  moveForward(); // Start by moving the car forward
}

void loop() {
  update_mpu_readings(); // Continuously update MPU6050 sensor readings
  sendData("false", yaw, distance, 0); // Send current data over serial
  if (Serial.available()) { 
    // Check if data is available on the serial port
    checkCommand(Serial.readStringUntil('\r')); // Read the command and process it
  }
}


/**
 * Function to set the car's path and control its movement.
 * This involves turning the car to a specific angle and then moving forward
 * for a specified distance while adjusting the speed based on the yaw angle.
 * @param initial_angle - The target yaw angle to turn the car to.
 * @param path_distance - The distance the car should travel after turning.
 * @param speed - The speed at which the car should move.
 */
void SetCarPath(signed int initial_angle, int path_distance, char speed) {
  update_mpu_readings();
  turnCar(initial_angle, speedTurn); // Turn the car to the specified angle
  moveForward(); // Start moving forward
  distance = 0; // Reset distance traveled
  steps = 0; // Reset step count

  // Continue moving until the car reaches the specified distance
  while (distance < path_distance) {
    // Adjust motor speeds based on the current yaw angle
    analogWrite(motorEN1, speed + (yaw- initial_angle)*speed_adjust_factor + speed_error_factor);
    analogWrite(motorEN2, speed - (yaw- initial_angle)*speed_adjust_factor - speed_error_factor);
    
    if (digitalRead(sensor)) { // Check if the sensor detects a step
      steps += 1;
      distance = (steps / 90.0) * 100; // Convert steps to distance
      while (digitalRead(sensor)); // Wait for the sensor to clear
    }
    update_mpu_readings();
  }
  sendData("true", yaw, distance, speed); // Send final status
  instantStop(); // Stop the car
}

/**
 * Function to send the current status of the car via serial communication.
 * @param carIsMoving - Indicates if the car is currently moving ("true" or "false").
 * @param angle - The current yaw angle of the car.
 * @param dis - The distance traveled by the car.
 * @param speed - The current speed of the car.
 */
void sendData(String carIsMoving, signed int angle, int dis, int speed) {
  Serial.print("status:carIsMoving=");
  Serial.print(carIsMoving);
  Serial.print("&dir=");
  
  // Determine the direction based on the angle (right or left)
  if (angle < 0) {
    Serial.print("r"); // Right
  } else {
    Serial.print("l"); // Left
  }
  
  Serial.print("&angle=");
  Serial.print(abs(angle)); // Send the absolute value of the angle
  Serial.print("&distance=");
  Serial.print(dis); // Send the distance traveled
  Serial.print("&speed=");
  Serial.print(speed); // Send the current speed
  Serial.print("\r"); // End of message
}

/**
 * Function to execute a sequence of movements based on the command received.
 * @param str - The command string indicating the sequence to execute (e.g., "moveSquare").
 */
void performSequence(String str) {
  Serial.print("executeSequence:success=true&status=inProgress\r");
  update_mpu_readings(); // Update sensor readings
  
  // Execute specific movement based on the command
  if (str == "moveSquare") {
    moveSquare(squareLength, squareSpeed);
  }
  if (str == "moveRectangle") {
    moveRectangle(rectangleLength, rectangleWidth, rectangleSpeed);
  }
  if (str == "moveTriangle") {
    moveTriangle(triangleLength, triangleSpeed);
  }

  Serial.print("executeSequence:success=true&status=completed\r");
}

/**
 * Function to parse and execute commands received via serial communication.
 * It determines whether the command is a sequence or a path and acts accordingly.
 * @param str - The command string received via serial communication.
 */
void checkCommand(String str) {
  update_mpu_readings(); // Update sensor readings

  // Check if the command is a sequence command
  if (str.indexOf(sequenceCommand) > -1) {
    str.remove(str.indexOf(sequenceCommand), str.indexOf(sequenceCommand) + sequenceCommand.length() + 6);
    performSequence(str); // Execute the sequence command
  }

  // Check if the command is a path command
  if (str.indexOf(pathCommand) > -1) {
    String param, val;
    str.remove(0, str.indexOf(pathCommand) + pathCommand.length() + 1);

    // Parse the parameters from the command string
    while (str.length() > 0) {
      update_mpu_readings();
      param = str.substring(0, str.indexOf(path_equalCommand)); // Get parameter name
      str.remove(0, str.indexOf(path_equalCommand) + 1);

      val = str.substring(0, str.indexOf(path_substringCommand)); // Get parameter value
      str.remove(0, str.indexOf(val) + val.length());

      // Assign values to the pathDetails array based on parameter names
      if (param == path_angleCommand) {
        pathDetails[0] = val.toInt(); // Set angle
      }
      
      if (param == path_angleDirCommand && val == "r") {
        pathDetails[0] = -pathDetails[0]; // Reverse angle direction if "r"
      }

      if (param == path_distanceCommand) {
        pathDetails[1] = val.toInt(); // Set distance
      }

      if (param == path_speedCommand) {
        pathDetails[2] = val.toInt(); // Set speed
      }

      str.remove(0, str.indexOf(path_substringCommand) + 1);
    }

    Serial.print("executePath:success=true&status=inProgress\r");
    SetCarPath(yaw + pathDetails[0], pathDetails[1], pathDetails[2]); // Execute the path command
    Serial.print("executePath:success=true&status=completed\r");
  }
}


/**
 * Function to move the car in a square pattern.
 * The car moves forward along the sides of a square, turning 90 degrees at each corner.
 * @param length - The length of each side of the square.
 * @param speed - The speed at which the car should move.
 */
void moveSquare(float length, char speed) {
  SetCarPath(yaw, length, speed);         // Move forward for the first side
  SetCarPath(yaw - 90, length, speed);    // Turn and move for the second side
  SetCarPath(yaw - 90, length, speed);    // Turn and move for the third side
  SetCarPath(yaw - 90, length, speed);    // Turn and move for the last side
  turnCar(yaw - 180, speedTurn);          // Final turn to complete the square
}

/**
 * Function to move the car in a triangle pattern.
 * The car moves forward along the sides of an equilateral triangle, turning 120 degrees at each corner.
 * @param length - The length of each side of the triangle.
 * @param speed - The speed at which the car should move.
 */
void moveTriangle(float length, char speed) {
  SetCarPath(yaw, length, speed);         // Move forward for the first side
  SetCarPath(yaw - 120, length, speed);    // Turn and move for the second side
  SetCarPath(yaw - 120, length, speed);    // Turn and move for the third side
  turnCar(yaw - 180, speedTurn);          // Final turn to complete the triangle
}

/**
 * Function to move the car in a rectangular pattern.
 * The car moves forward along the sides of a rectangle, turning 90 degrees at each corner.
 * @param length - The length of the rectangle.
 * @param width - The width of the rectangle.
 * @param speed - The speed at which the car should move.
 */
void moveRectangle(float length, float width, char speed) {
  SetCarPath(yaw, length, speed);         // Move forward for the first side (length)
  SetCarPath(yaw - 90, width, speed);     // Turn and move for the second side (width)
  SetCarPath(yaw - 90, length, speed);    // Turn and move for the third side (length)
  SetCarPath(yaw - 90, width, speed);     // Turn and move for the fourth side (width)
  turnCar(yaw - 90, speedTurn);           // Final turn to complete the rectangle
}

/**
 * Function to turn the car to a specific yaw angle.
 * The car will continuously adjust its direction until the desired angle is reached.
 * @param ang - The target yaw angle to turn the car to.
 * @param speed - The speed at which the car should turn.
 */
void turnCar(signed int ang, int speed) {
  while (1) {
    update_mpu_readings();
    if (yaw > (ang + 1)) { // If the car needs to turn right
      moveRight();         // Turn the car to the right
      applyCarSpeed(speed); // Apply the turning speed
    } else if (yaw < (ang - 1)) { // If the car needs to turn left
      moveLeft();          // Turn the car to the left
      applyCarSpeed(speed); // Apply the turning speed
    } else {
      instantStop(); // Stop the car when the desired angle is reached
      break;
    }
  }
}

/**
 * Function to apply a specified speed to both motors of the car.
 * @param speed - The speed value to apply to the motors (0-255).
 */
void applyCarSpeed(int speed) {
  analogWrite(motorEN1, speed); // Set speed for the right motor
  analogWrite(motorEN2, speed); // Set speed for the left motor
}

/**
 * Function to immediately stop the car by setting all motor pins high.
 */
void instantStop() {
  digitalWrite(motorR1, HIGH); 
  digitalWrite(motorR2, HIGH); 
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, HIGH);
}

// Movement control functions

/**
 * Function to move the car forward.
 * The right and left motors are set to rotate in the forward direction.
 */
void moveForward() {
  digitalWrite(motorR1, LOW); 
  digitalWrite(motorR2, HIGH); 
  digitalWrite(motorL1, LOW); 
  digitalWrite(motorL2, HIGH);
}

/**
 * Function to turn the car to the right.
 * The right motor moves backward and the left motor moves forward.
 */
void moveRight() {
  digitalWrite(motorR1, HIGH); 
  digitalWrite(motorR2, LOW); 
  digitalWrite(motorL1, LOW); 
  digitalWrite(motorL2, HIGH);
}

/**
 * Function to move the car backward.
 * The right and left motors are set to rotate in the reverse direction.
 */
void moveBackward() {
  digitalWrite(motorR1, HIGH); 
  digitalWrite(motorR2, LOW); 
  digitalWrite(motorL1, HIGH); 
  digitalWrite(motorL2, LOW);
}

/**
 * Function to turn the car to the left.
 * The right motor moves forward and the left motor moves backward.
 */
void moveLeft() {
  digitalWrite(motorR1, LOW); 
  digitalWrite(motorR2, HIGH); 
  digitalWrite(motorL1, HIGH); 
  digitalWrite(motorL2, LOW);
}


void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
}


void update_mpu_readings()
{
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX ~(-5.25) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY; // AccErrorY ~(2.66)
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX; // GyroErrorX ~(0.21)
  GyroY = GyroY - GyroErrorY; // GyroErrorY ~(0.11)
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ ~ (-0.75)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
}
