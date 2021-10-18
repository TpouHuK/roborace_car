#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

#define SENSOR_AMOUNT 8

#define SERVO_PIN 10
#define MOTOR_PIN 6
#define TAHO_PIN 2

#define NEUTRAL_POINT 96
#define MIN_POINT 60  
#define MAX_POINT 130

#define NEUTRAL_SPEED 90
#define MIN_SPEED 95
#define MAX_SPEED 180
#define MIN_BSPEED 85

#define IR_PIN0 A1 //TODO
#define IR_PIN1 A0 //TODO
#define IR_PIN2 A3 //TODO

#define TCAADDR 0x70

void tca_select(uint8_t i) {
 if (i > 7) return;
 Wire.beginTransmission(TCAADDR);
 Wire.write(1 << i);
 Wire.endTransmission();
}

class Car {
  private:
    Servo steer_servo;
    Servo motor;
    VL53L0X laser_sensors[6];
    
  public:
    const int sensor_amount = SENSOR_AMOUNT;
    void write_speed(int s);
    void write_steer(int s);
    int* read_sensors();
    void init();
};

volatile unsigned long last_turnover = 0, turnover = 0, turnover_time = 0, cur_time = 0;
void taho_interrupt(){
  turnover =  (long)(micros() - last_turnover);
  if (turnover > 20000){
    turnover_time = turnover;
    last_turnover = micros();
  }
}

const float pi = 3.141592653589;

float get_speed(){
  cur_time = (long)(micros() - last_turnover);
  cur_time = max(cur_time, turnover_time);
  return 2.0*pi/((float)cur_time/1000000)*0.035/2;
  //return turnover_time;
}

void Car::init(){
  
  steer_servo.attach(SERVO_PIN);
  motor.attach(MOTOR_PIN);
  
  pinMode(2, INPUT);
  attachInterrupt(0, taho_interrupt, RISING);
  
  Wire.begin();

  for (byte i = 0; i < 6; i++){
    tca_select(i+2);
    laser_sensors[i].init();
    // lower the return signal rate limit (default is 0.25 MCPS)
    //laser_sensors[i].setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    //laser_sensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    //laser_sensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    laser_sensors[i].setMeasurementTimingBudget(20000);
    laser_sensors[i].startContinuous();
  }
  
  write_steer(0);
  write_speed(0);
}

void Car::write_speed(int s){
  s = constrain(s, -1000, 1000);
  if (s > 0) {
    s = map(s, 1, 1000, MIN_SPEED, MAX_SPEED);
  } else if (s < 0) {
    s = map(s, -1000, -1, 0, MIN_BSPEED);
  } else {
    s = NEUTRAL_SPEED;
  }
  motor.write(s);
}

const int SAMPLE_NUM = 3;

int compare_int (const void * a, const void * b){
  if ( *(int*)a < *(int*)b ) return -1;
  if ( *(int*)a == *(int*)b ) return 0;
  if ( *(int*)a > *(int*)b ) return 1;
  return 0;
}

float read_sensor(int Sensor, int model) {
  int raw[SAMPLE_NUM];
  for (int i = 0; i < SAMPLE_NUM; i++){
    raw[i] = analogRead(Sensor);
  }
  qsort(raw, SAMPLE_NUM, sizeof(int), compare_int);

  float volts = (raw[SAMPLE_NUM / 2] + 0.5) * 5.0 / 1024.0;

  if (model == 20150) {
    float val = 60.374 * pow(volts, -1.16);
    return constrain(val, 20.0, 150.0) * 10.0;
  } else if (model == 1080) {
    float val = 29.988 * pow(volts, -1.173);
    return constrain(val, 10.0, 80.0) * 10.0;
  }
  return 0;
}

int* Car::read_sensors(){
  static int values[SENSOR_AMOUNT];

  for (int i = 0; i < 6; i++){
    tca_select(i+2);
    int rv = laser_sensors[i].readRangeContinuousMillimeters();
    values[i] = constrain(rv, 0, 1000);
    //values[i] = laser_sensors[i].readRangeSingleMillimeters();
  }
  values[6] = read_sensor(A7, 20150);
  values[7] = read_sensor(A0, 20150);
  
  return values;
}

void Car::write_steer(int s){
  s = s * -1;
  s = constrain(s, -1000, 1000);
  
  if (s < 0){
    s = map(s, -1000, 0, MIN_POINT, NEUTRAL_POINT);
  } else {
    s = map(s, 0, 1000, NEUTRAL_POINT, MAX_POINT);
  }
  steer_servo.write(s);
}
