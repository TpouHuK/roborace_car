#include "big_car.h"
#include "tests.h"

Car car;

void setup() {
  car.init();
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  float* sensors = car.read_sensors();

  // <0 /1 ^2 \3 >4
  float mid_distance = sensors[1] - sensors[3];
  float turn = mid_distance * 2;

  float max_constrain = -33 * (-25 + sensors[2] / 10) + 2000;
  max_constrain = constrain(max_constrain, 300, 1000);
  turn = constrain(turn, -max_constrain, max_constrain);

  static bool fast = false;
  static float turn_acc = 0;
  turn_acc = turn_acc * 0.8 + abs(turn * turn);

  float spd;
  if (abs(turn_acc) < 2250000) {
    if (not fast) {
      // Speed up
      car.write_speed(400);
      delay(150);
    }
    digitalWrite(13, HIGH);
    spd = 130;
    fast = true;
  } else {
    if (fast) {
      // Slow down
      car.write_speed(-100);
      delay(150);
    }
    digitalWrite(13, LOW);
    spd = 100;
    fast = false;
  }

  if (sensors[2] < 80) {
    car.write_speed(0);
    car.write_steer(-turn);
    car.write_speed(-200);
    while (sensors[2] < 250) {
        sensors = car.read_sensors();
    };
    delay(400);
    car.write_speed(200);
    delay(600);
  }

  car.write_speed(spd);
  car.write_steer(turn);
}
