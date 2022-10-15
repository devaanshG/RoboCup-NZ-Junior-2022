#include <NewPing.h>
#include <Wire.h>
#include <MPU9250_asukiaaa.h>

//PINS
#define LEFT_MOTOR_PIN_FWD 2
#define LEFT_MOTOR_PIN_REV 7
#define RIGHT_MOTOR_PIN_FWD 4
#define RIGHT_MOTOR_PIN_REV 8


#define LEFT_MOTOR_PIN_EN 3//PWN
#define RIGHT_MOTOR_PIN_EN 10//PWN

#define HARD_TURN_MULTIPLIER 2

#define FORWARD_SENSOR_PIN A3
#define BACKWARDS_SENSOR_PIN A7
#define FORWARD_LEFT_SENSOR_PIN A6
#define FORWARD_RIGHT_SENSOR_PIN A2
#define LEFT_SENSOR_PIN A0
#define RIGHT_SENSOR_PIN A1


//CONSTANTS
//#define MOTOR_RPM 250
//#define WHEEL_CIRCUMFERENCE 20.42//cm
//#define DISTANCE_CENTER_WHEELS_SS 15//cm
//#define DISTANCE_CENTER_WHEELS_OS 10//cm
//dxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//float MOTOR_SPEED = MOTOR_RPM*60*WHEEL_CIRCUMFERENCE;//per second
//float HARD_TURN_CIRCUMFERENCE = PI*DISTANCE_CENTER_WHEELS_OS;
//float HARD_TURN_TIME = HARD_TURN_CIRCUMFERENCE/MOTOR_SPEED;//in seconds

#define SENSOR_CLEAR_THRESHOLD 15//CM
#define SENSOR_MAX_DISTANCE 400//cm
#define PING_COOLDOWN 20//ms

#define TURN_DEG_SPEED 255
#define TURN_DEG_ERROR 1//radians, how close is good enough
#define TURN_MS 500
#define REV_MS 200//ms

#define MPU_ADDRESS 0x68
int x_movement;
int z_rotation;

//VARIABLES
int sensor_distances[6];

MPU9250_asukiaaa mpu;

NewPing forward(FORWARD_SENSOR_PIN, FORWARD_SENSOR_PIN, SENSOR_MAX_DISTANCE);
NewPing backwards(BACKWARDS_SENSOR_PIN, BACKWARDS_SENSOR_PIN, SENSOR_MAX_DISTANCE);
NewPing f_left(FORWARD_LEFT_SENSOR_PIN, FORWARD_LEFT_SENSOR_PIN, SENSOR_MAX_DISTANCE);
NewPing f_right(FORWARD_RIGHT_SENSOR_PIN, FORWARD_RIGHT_SENSOR_PIN, SENSOR_MAX_DISTANCE);
NewPing left(LEFT_SENSOR_PIN, LEFT_SENSOR_PIN, SENSOR_MAX_DISTANCE);
NewPing right(RIGHT_SENSOR_PIN, RIGHT_SENSOR_PIN, SENSOR_MAX_DISTANCE);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(LEFT_MOTOR_PIN_FWD, OUTPUT);
  pinMode(LEFT_MOTOR_PIN_REV, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_FWD, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_REV, OUTPUT);
  
  pinMode(LEFT_MOTOR_PIN_EN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN_EN, OUTPUT);
  
  mpu.setWire(&Wire);
  mpu.beginGyro();
  mpu.beginAccel();
}

void loop() {
  Serial.println("updating data...");
  update_sensor_data();
  mpu.gyroUpdate();
  mpu.accelUpdate();
  z_rotation = mpu.gyroZ();
  Serial.println("Gyro Z_Rotation: " + z_rotation);
  x_movement = mpu.accelX();
  Serial.println("Gyro X_Movement: " + x_movement);
  Serial.println("update completed!");
  
  for(int i=0; i<=5; i++){//if theres nowhere to go it will just continue on its way
    if(sensor_distances[i] <= SENSOR_CLEAR_THRESHOLD && sensor_distances[i] > 0){ //It's important to write > 0, as some really large distances are just read as 0 by the sensors. In addition, the reading will never be 0 otherwise.
      Serial.println(i);
      switch (i){
        case 0://forward
          reverse_for(REV_MS);
          turn_until_clear();
          break;
        case 1://back
          break;
        case 2://left
          if(sensor_distances[3] > SENSOR_CLEAR_THRESHOLD || sensor_distances[3] == 0) {
            turn_for(TURN_MS*HARD_TURN_MULTIPLIER, false);
          }
          else {
            reverse_until_clear();
          }
          break;
        case 3://right
          if(sensor_distances[2] > SENSOR_CLEAR_THRESHOLD || sensor_distances[2] == 0) {
            turn_for(TURN_MS*HARD_TURN_MULTIPLIER, true);
          }
          else {
            reverse_until_clear();
          }
          break;
        case 4:
          if(0 < sensor_distances[i] && sensor_distances[i] <= SENSOR_CLEAR_THRESHOLD/2){
            turn_for(TURN_MS, false);//turn_deg(-45);
          }
          break;
        case 5:
          if(0 < sensor_distances[i] && sensor_distances[i] <= SENSOR_CLEAR_THRESHOLD/2){
            turn_for(TURN_MS, true);//turn_deg(-45);
          }
          break;
        default://no programmed output
          break;
      }
    }
   
    else {
      change_movement(255, 255);
    }
  }

//  if (z_rotation == 0 && x_movement == 0){
//    Serial.println("Stuck");
//    reverse_for(200);
//    turn_until_clear();
//  }
}

void update_sensor_data() {
  delay(PING_COOLDOWN);
  sensor_distances[0] = forward.ping_cm();
//  Serial.println("pinged forward");
//  Serial.println(sensor_distances[0]);
  delay(PING_COOLDOWN);
  sensor_distances[1] = backwards.ping_cm();
//  Serial.println("pinged backwards");
//  Serial.println(sensor_distances[1]);
  delay(PING_COOLDOWN);
  sensor_distances[2] = f_left.ping_cm();
//  Serial.println("pinged f-left");
//  Serial.println(sensor_distances[2]);
  delay(PING_COOLDOWN);
  sensor_distances[3] = f_right.ping_cm();
//  Serial.println("pinged f-right");
//  Serial.println(sensor_distances[3]);
  delay(PING_COOLDOWN);
  sensor_distances[4] = left.ping_cm();
//  Serial.println("pinged left");
//  Serial.println(sensor_distances[4]);
  delay(PING_COOLDOWN);
  sensor_distances[5] = right.ping_cm();
//  Serial.println("pinged right");
//  Serial.println(sensor_distances[5]);
}

void turn_until_clear(){
  Serial.println("Turn until clear");
  while(0 < sensor_distances[0] && sensor_distances[0] <= SENSOR_CLEAR_THRESHOLD){
    Serial.println("Turning");
    update_sensor_data();
    if (sensor_distances[4] > SENSOR_CLEAR_THRESHOLD || sensor_distances[4] == 0) {
      change_movement(255, -255); 
    }
    else if (sensor_distances[5] > SENSOR_CLEAR_THRESHOLD || sensor_distances[5] == 0) {
      change_movement(-255, 255); 
    }
    else {
      reverse_until_clear();
    }
  }
  change_movement(0, 0);
}

void reverse_until_clear() {
  Serial.println("Reverse until clear");
  while(0 < sensor_distances[4] && sensor_distances[4] <= SENSOR_CLEAR_THRESHOLD){
    Serial.println("Reversing...");
    update_sensor_data();
    reverse_for(100);

    if(sensor_distances[4] > SENSOR_CLEAR_THRESHOLD || sensor_distances[4] == 0) {
      reverse_for(2000);
      turn_for(1200, true);
      break;
    }
    else if(sensor_distances[5] > SENSOR_CLEAR_THRESHOLD || sensor_distances[5] == 0) {
      reverse_for(2000);
      turn_for(1200, false);
      break;
    }
  }
}

void reverse_for(int ms){//reverses for ms unless there is an obstical
  Serial.println("Reverse For");
  change_movement(-255, -255);
  int begin_millis = millis();
  while(begin_millis + ms >= millis()){
    update_sensor_data();
    if((0 < sensor_distances[1] && sensor_distances[1] <= SENSOR_CLEAR_THRESHOLD)){
      change_movement(0, 0);
      break;
    }
  }
  change_movement(0, 0);
}


void turn_deg(int deg){
  Serial.println("Turn Deg");
  mpu.gyroUpdate();
  double current_rad = 0;
  double target_rad = deg*PI/180;
  int prev_millis = millis();
  while(abs(current_rad-target_rad)>TURN_DEG_ERROR){
    Serial.println("Turning...");
    mpu.gyroUpdate();  
    int current_millis = millis();
    current_rad += mpu.gyroZ()*(current_millis-prev_millis)/1000;
    current_rad = fmod(current_rad, (2*PI));
    if(current_rad-target_rad<target_rad-current_rad){
      change_movement(TURN_DEG_SPEED, -TURN_DEG_SPEED);
    }else{
      change_movement(-TURN_DEG_SPEED,  TURN_DEG_SPEED);
    }
    prev_millis = current_millis;
  }
  change_movement(0, 0);
}

void turn_for(int ms, bool clockwise){
  Serial.println("Turn For");
//  int start_ms;
  if(clockwise){
//    start_ms = millis();
    change_movement(255, -255);
  }else{
//    start_ms = millis();
    change_movement(-255, 255);
  }
//  while(start_ms+ms >= millis()){}//delay
  delay(ms);
  change_movement(0, 0);
}

void change_movement(int l, int r) { //speed can be negative, char is a signed byte
  Serial.println("Changing Movement");
  analogWrite(RIGHT_MOTOR_PIN_EN, abs(r));
  analogWrite(LEFT_MOTOR_PIN_EN, abs(l));
  if(r > 0){
    digitalWrite(RIGHT_MOTOR_PIN_FWD, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN_REV, LOW);
  } else{
    digitalWrite(RIGHT_MOTOR_PIN_FWD, LOW);
    digitalWrite(RIGHT_MOTOR_PIN_REV, HIGH);
  }
  if(l > 0){
    digitalWrite(LEFT_MOTOR_PIN_FWD, HIGH);
    digitalWrite(LEFT_MOTOR_PIN_REV, LOW);
  } else{
    digitalWrite(LEFT_MOTOR_PIN_FWD, LOW);
    digitalWrite(LEFT_MOTOR_PIN_REV, HIGH);
  }
}
