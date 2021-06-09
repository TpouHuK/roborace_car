#include "big_car.h"
#include "tests.h"

Car car;

void setup() {
  car.init();
  Serial.begin(9600);
}

void loop() {
  car.write_steer(1000);
  return;
  float* sensors = car.read_sensors();
  
  // <0 /1 ^2 \3 >4
  float mid_distance = sensors[0] - sensors[4];
  float turn_distance = sensors[1] - sensors[3];

  const float P1 = 2;
  const float P2 = 0.5;
  
  float turn;
  if (abs(mid_distance) > 400){
    turn = turn_distance * P1;
  } else {
    turn = turn_distance * P2;
    turn = constrain(turn, -500, 500);
  }
  //Serial.print("mid: ");
  //Serial.println(mid_distance);
  //Serial.print("turn: ");
  //Serial.println(turn_distance);
  
  //print_sensors(car);
  //delay(1000);
  //car.write_speed(100);
  car.write_steer(turn);
}
