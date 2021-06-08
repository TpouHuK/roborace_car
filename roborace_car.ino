#include "mini_car.h"

Car car;

void setup() {
  car.init();
  Serial.begin(9600);
  
}

void loop() {
  // -1000 full back, 1000 full forward, 0 neutral
  car.write_speed(0); // -1000..1000
  
  // -1000 full left, 1000 full right, 0 straight
  car.write_steer(0); // -1000..1000
  
  float* sensors = car.read_sensors(); // Array of 4 floats
  
  sensors[0]; // Left
  sensors[1]; // Middle
  sensors[2]; // Right
  sensors[3]; // Front button, 0 is pressed, 1 is released
}
