#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

#define SENSOR_AMOUNT 5 

#define SERVO_PIN 5

#define NEUTRAL_POINT 90
#define MIN_POINT 60
#define MAX_POINT 130

#define INA 8
#define INB 9
#define PWM 11

#define IR_PIN0 A1
#define IR_PIN1 A0
#define IR_PIN2 A3
#define IR_PIN3 A2

class Car {
  private:
    Servo steer_servo;
    VL53L0X laser_sensor;
    
  public:
    const int sensor_amount = SENSOR_AMOUNT;
    void write_speed(int s);
    void write_steer(int s);
    float* read_sensors();
    void init();
};

void Car::init(){
  
  steer_servo.attach(SERVO_PIN);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  
  Wire.begin();
 
  laser_sensor.init();
  laser_sensor.startContinuous();
  
  write_steer(0);
  write_speed(0);
}

void Car::write_speed(int s){
  s = constrain(s, -1000, 1000);
  s = map(s, -1000, 1000, -255, 255);
  if (s > 0){
    digitalWrite(INA, HIGH);
    digitalWrite(INB, LOW);
  } else {
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
  }
  analogWrite(PWM, abs(s));
}

const int SAMPLE_NUM = 5;

int compare_int (const void * a, const void * b){
  if ( *(int*)a < *(int*)b ) return -1;
  if ( *(int*)a == *(int*)b ) return 0;
  if ( *(int*)a > *(int*)b ) return 1;
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
}

float* Car::read_sensors(){
  static float values[SENSOR_AMOUNT];
  
  values[0] = read_sensor(IR_PIN0, 1080);
  values[1] = read_sensor(IR_PIN1, 20150);
  values[2] = laser_sensor.readRangeContinuousMillimeters();
  values[2] = constrain(values[2], 0, 1000);
  values[3] = read_sensor(IR_PIN2, 20150);
  values[4] = read_sensor(IR_PIN3, 1080);
  
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

