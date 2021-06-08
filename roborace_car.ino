#include "mini_car.h"
#include "tests.h"

Car car;

void setup() {
  car.init();
  Serial.begin(9600);
  while (car.read_sensors()[3] == 1){}
  while (car.read_sensors()[3] == 0){}
}

void loop() {
  print_sensors(car);
  delay(1000);
  return;
  float* sensors = car.read_sensors();
  // 0 - Left
  // 1 - Middle
  // 2 - Right
  // 3 - Front button, 0 is pressed, 1 is released

  int steer = (sensors[2] - sensors[0]) / 2;
  
  static bool fast = false;
  
  if (sensors[1] > 1200) {
    car.write_speed(200);
    fast = true;
  } else {
    if (fast) {
      car.write_speed(-80);
      delay(100);
    }
    car.write_speed(110);
    fast = false;
  }
  car.write_steer(steer);

  if (sensors[3] == 0.0){
    car.write_speed(0);
    delay(5000);
  }
}
