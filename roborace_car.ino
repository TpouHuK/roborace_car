#include "mini_car.h"
//#include "tests.h"
#define LOOP_TIME 30;

Car car;

void setup() {
  car.init();
  Serial.begin(9600);
  Serial.println("program started!");
  delay(3000);
}

unsigned long next_loop = 0;
bool speed = false;


void loop() {
  unsigned long cur_time = millis();
  if (cur_time > next_loop) {
    next_loop = next_loop+LOOP_TIME;
    next_loop = max(cur_time, next_loop);

    int* s = car.read_sensors();

    int front_dist = s[1];
    int diff = s[2] - s[0];
    
    if (s[4] < 1800 || s[1] < 300) {
      car.write_speed(80);
      car.write_steer(diff*20);
    } else {
      car.write_speed(200);
      car.write_steer(0);
    }
    
    /* sprintf(data, "a%04db%04dc%04dd%04de%04df%04d", front_dist, s[1], s[4], 0, 0, 0);
    Serial.println();
    Serial.println(data);
    Serial.flush(); */
    
    /*int front_dist = (s[4] + s[1]) / 2;
    
    if (front_dist < 600) {
      car.write_speed(0);
    } else {
      car.write_speed(50);
    }

    
    int turn_force = (s[0]+s[3]/2) - (s[5]+s[2]/2);
    int diff;
    if (front_dist > 900){
      diff = 0;
    } else {
      diff =  turn_force;  
    }
    car.write_steer(diff*-12);
    */
  }
}
