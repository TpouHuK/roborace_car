/*
Arduino по умолчанию все считает в int. Поэтому в строку turnover = micros() - last_turnover; нужно записать так: turnover = (long) (micros() - last_turnover); Ну и в условии соответственно тоже. Иначе будут переполнения и соответственно глюки. Сам по таким граблям ходил :-)
*/
#include "big_car.h"
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

unsigned int afk_counter = 0;
float sum_turn = 0;

void go_back() {
  car.write_speed(0);
      while (get_speed() > 0.0001) {}
      car.write_speed(-100);
      delay(100);
      car.write_speed(0);
      delay(100);
      car.write_speed(-100);
      delay(700);
      car.write_speed(0);
      while (get_speed() > 0.0001) {}
}

void loop() {
  unsigned long cur_time = millis();
  if (cur_time > next_loop) {
    next_loop = next_loop+LOOP_TIME;
    next_loop = max(cur_time, next_loop);

    int* s = car.read_sensors();
    float speed = get_speed();
    
    int front_dist = (s[4] + s[1]) / 2;

    if (front_dist > 100){
      if (speed < 1.3) {
        car.write_speed(80);
      } else {
        car.write_speed(0);
      }
    } else {
      car.write_speed(0);
    }

    
    int turn_force = (s[0]+s[3]/2) - (s[5]+s[2]/2);
    int diff;
    
    if (front_dist > 999){
      diff =  turn_force/20;
    } else {
      diff =  turn_force;  
    }
    
    car.write_steer(diff*-12);

    float cur_turn = diff*-12;
    cur_turn = constrain(cur_turn, -1000, 1000);

    sum_turn += cur_turn;
    sum_turn = constrain(sum_turn, -9999999, 2000);

    if (sum_turn < -60000) {
      sum_turn = 0;
      car.write_speed(-100);
      delay(1000);
      car.write_steer(diff*12);
      go_back();
      car.write_steer(diff*-12);
      car.write_speed(50);
      delay(1000);
    }
    
    if (front_dist < 100 && speed < 0.0001 ) {
      go_back();
    }
    
    if (speed < 0.0001) {
      afk_counter += 1;
    } else {
      afk_counter = 0;
    }

    if (afk_counter > 170) {
      go_back();
      afk_counter = 0;
    }
  }
}
