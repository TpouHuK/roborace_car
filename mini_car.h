#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

#define BUTTON_PIN 12
#define XSHUT_1 7
#define XSHUT_2 6
#define XSHUT_3 4

#define SENSOR_AMOUNT 4 // 3 IR and button

#define SERVO_PIN 5

#define NEUTRAL_POINT 100
#define MIN_POINT 65
#define MAX_POINT 120

#define INA 9
#define INB 10
#define PWM 11

class Car {
  private:
    Servo steer_servo;
    VL53L0X Sensor1;
    VL53L0X Sensor2;
    VL53L0X Sensor3;
  
  public:
    const int sensor_amount = SENSOR_AMOUNT;
    void write_speed(int s);
    void write_steer(int s);
    float* read_sensors();
    void init();
};

void Car::init(){
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  steer_servo.attach(SERVO_PIN);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);

  pinMode(XSHUT_1, OUTPUT);
  pinMode(XSHUT_2, OUTPUT);
  pinMode(XSHUT_3, OUTPUT);  
  digitalWrite(XSHUT_1, LOW);
  digitalWrite(XSHUT_2, LOW);
  digitalWrite(XSHUT_3, LOW);
  delay(10);
  
  Wire.begin();
  pinMode(XSHUT_1, INPUT); // XSHUT isn't 5v tolerant
  delay(10); // Time for sensor boot
  Sensor1.setAddress(41+1);

  pinMode(XSHUT_2, INPUT);
  delay(10);
  Sensor2.setAddress(41+2);

  pinMode(XSHUT_3, INPUT);
  delay(10);
  Sensor3.setAddress(41+3);
  
  Sensor1.init();
  Sensor2.init();
  Sensor3.init();
  Sensor1.startContinuous();
  Sensor2.startContinuous();
  Sensor3.startContinuous();
  
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

float* Car::read_sensors(){
  static float values[SENSOR_AMOUNT];
  values[0] = Sensor1.readRangeContinuousMillimeters();
  values[1] = Sensor2.readRangeContinuousMillimeters();
  values[2] = Sensor3.readRangeContinuousMillimeters();
  values[3] = digitalRead(BUTTON_PIN);
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

