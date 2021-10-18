#include "big_car.h"
#define LOOP_TIME 30;

Car car;

void setup() {
  car.init();
  delay(3000);
}

unsigned long next_loop = 0;
bool speed = false;

unsigned int afk_counter = 0;
float sum_turn = 0;

void go_back() {
  car.write_speed(0);
  while (get_speed() > 0.4) {}
  car.write_speed(-100);
  delay(100);
  car.write_speed(0);
  delay(100);
  car.write_speed(-100);
  delay(700);
  car.write_speed(0);
  while (get_speed() > 0.4) {}
}

float dist = 0;
unsigned long dt;
bool no_back = false;

void loop() {
  unsigned long cur_time = millis();
  if (cur_time > next_loop) {
    dt = millis() - next_loop + LOOP_TIME;
    next_loop = next_loop + LOOP_TIME;
    next_loop = max(cur_time, next_loop);

    int* s = car.read_sensors();
    /* char data[100];
      sprintf(data, "a%04db%04dc%04dd%04de%04df%04d", s[6], s[7], s[2], s[3], s[4], s[5]);
      Serial.println();
      Serial.println(data);
      Serial.  flush();
      return; */
    
    float speed = get_speed();
    dist += speed * dt / 1000.0;

    if (dist < 14.5){
      if (speed < 4.5){
        car.write_speed(190);
      } else {
        car.write_speed(0);
      }
    } else {
      if (speed > 0.1){ 
        if (not no_back) {
          car.write_speed(-500);
        } else {
          car.write_speed(0);
        }
      } else {
        car.write_speed(0);
        no_back = true;
      }
    }

    float p = 1;
    float d = 20;
    static float last_error = 0;
    float error = (s[7] - s[6]);
    float turn = error * p + (error - last_error)*d;
    turn = constrain(turn, -200, 200);
    last_error = error;
    car.write_steer(turn);
    
    /*
    int front_dist = (s[4] + s[1]) / 2;

    if (front_dist > 999){
      if (front_dist > 100) {
        if (speed < 2.3) {
          if (digitalRead(2)) {
            car.write_speed(50);
        
          
          } else {
            car.write_speed(150);
          }
        } else {
          if (speed < 2.3) {
            car.write_speed(100);
          } else {
            if (speed < 3.0) {
              car.write_speed(50);
            } else {
              car.write_speed(0);
            }
          }
        }
      } else {
        car.write_speed(0);
      }
    } else {
      if (front_dist > 100) {
        if (speed < 0.4) {
          if (digitalRead(2)) {
            car.write_speed(50);
          } else {
            car.write_speed(150);
          }
        } else {
          if (speed < 1.3) {
            car.write_speed(100);
          } else {
            if (speed > 1.5) {
              car.write_speed(-100);
            } else {
              car.write_speed(0);
            }
          }
        }
      } else {
        car.write_speed(0);
      }
    }


    
    int turn_force = (s[6]) - (s[7]);
    
    float cur_turn = turn_force * -12;
    if (front_dist > 950) {
      cur_turn =  constrain(cur_turn, -50, 50);
    } else {
      //cur_turn *= 3;
    if (s[6] < s[7]) {
      cur_turn = 1000;
    } else {
      cur_turn = -1000;
    }
    cur_turn = constrain(cur_turn, -1000, 1000);
    }
    car.write_steer(cur_turn);

    sum_turn += cur_turn;
    sum_turn = constrain(sum_turn, -9999999, 60000);

    if (sum_turn < -60000) {
      sum_turn = 0;
      car.write_speed(-100);
      delay(1000);
      car.write_steer(cur_turn * -1);
      go_back();
      car.write_steer(cur_turn);
      car.write_speed(50);
      delay(1000);
    }

    if (front_dist < 100 && speed < 0.0001 ) {
      go_back();
    }

    if (speed < 0.1) {
      afk_counter += 1;
    } else {
      afk_counter = 0;
    }

    if (afk_counter > 30) {
      go_back();
      afk_counter = 0;
    } */
  }
}
