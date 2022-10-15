#include <Wire.h>

const int MPU_addr = 0x68; // MPU I2C address // 고유 레지스터
float AccX, AccY, AccZ; // 가속도 센서
float GyroX, GyroY, GyroZ; // 자이로 센서
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ; // 계산된 각도
float roll, pitch, yaw; // 계산된 roll, pitch, yaw
float elapsedTime, currentTime, previousTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 시리얼 통신 시작
  
  //Serial.println("CLEARDATA"); //-->추가
  //Serial.println("LABEL,Temp,Humi"); //-->추가

  Wire.begin(); //I2C 통신 시작
  Wire.beginTransmission(MPU_addr); //MPU6050_address로 통신 시작 (0x68)
  Wire.write(0x6B); //PWM_MGMT_1 레지스터로 통신
  Wire.write(0x00); //0입력, MPU6050 초기화
  Wire.endTransmission(true); //통신 종료
  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //=====================가속도 센서========================//
  Wire.beginTransmission(MPU_addr); //MPU_6050 adress로 통신 시작 (0x68)
  Wire.write(0x3B); // 가속도센서 데이터 레지스터 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true); // 6개 레지스터값 읽는다 : 각 축은 2개의 레지스터에서 읽는다.
  // += 2g 범위 값 사용 위해 raw값을 16384로 나눈다 (datasheet 참고)
  AccX = (Wire.read() << 8  | Wire.read()) / 16384.0; // X-axis Value
  AccY = (Wire.read() << 8  | Wire.read()) / 16384.0; // Y-axis Value
  AccZ = (Wire.read() << 8  | Wire.read()) / 16384.0; // Z-axis Value
  // 가속도센서 raw값을 통해 각도값 계산
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI);
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI);

  //==================== 자이로 센서 ========================//
  previousTime = currentTime; // 이전 loop에서의 시간 저장
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000; // 지난 시간을 초 단위로 계산
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x43); // 자이로 센서 데이터 레지스터 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true); // 3개의 축  각각 2개의 레지스터에서 읽는다. 
  // +-250deg/s 범위 값 사용을 위해 raw값을 131.0d으로 나눈다. (datasheet 참고)
  GyroX = (Wire.read() << 8  | Wire.read()) / 131.0; // X-axis Value
  GyroY = (Wire.read() << 8  | Wire.read()) / 131.0; // Y-axis Value
  GyroZ = (Wire.read() << 8  | Wire.read()) / 131.0; // Z-axis Value
  // 자이로 센서 raw 값을 통해 각도값 계산
  gyroAngleX = gyroAngleX + GyroX * elapsedTime;
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  gyroAngleY = gyroAngleY - 0.24 * elapsedTime;  // 일단 바꿈
  yaw = yaw + GyroZ * elapsedTime;
  //==================== 상보 필터 ========================//
  float alpha = 0.98;
  //deg = atan2(AccX, AccZ) * 180 / PI;
  //roll = (alpha * (roll + (GyroX * 0.001))) + (1-alpha)*deg;
  roll = alpha * gyroAngleX + (1-alpha) * accAngleX;
  pitch = alpha * gyroAngleY + (1-alpha) * accAngleY;
  


  //==================== 출력 ========================//

  Serial.print(accAngleY);
  Serial.print(',');
  Serial.println(gyroAngleY);

 
}
