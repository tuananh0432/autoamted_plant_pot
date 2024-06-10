#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
/* Địa chỉ của DS1307 */
const byte DS1307 = 0x68;
/* Số byte dữ liệu sẽ đọc từ DS1307 */
const byte NumberOfFields = 7;
/* khai báo các biến thời gian */
int second, minute, hour, day, wday, month, year;
/* Khai báo đối tượng LCD */
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;
const int lightSensorPin = 4;
int lightSensorState = LOW;
int servoAngle = 0;
int coverCount = 0;
unsigned long previousTime = 0;
const unsigned long interval = 30000;  // Thời gian giữa mỗi lần quay (30
giây)
void rotateServo(int angle);
void setup() {
  servo.attach(9);
  pinMode(lightSensorPin, INPUT);
  rotateServo(0);
  Wire.begin();
  /* cài đặt thời gian cho module */
  setTime(12, 30, 45, 1, 8, 2, 15);  // 12:30:45 CN 08-02-2015
  lcd.init();                        // Khởi tạo LCD
  lcd.backlight();                   // Bật đèn nền LCD
  previousTime = millis();           // Khởi tạo thời gian trước đó
}
void loop() {
  26 unsigned long currentTime = millis();
  if (currentTime - previousTime >= interval) {
    rotateServo(180 - servoAngle);  // Quay Servo ngược lại góc hiện tại
    previousTime = currentTime;     // Cập nhật thời gian trước đó
  }
  int currentLightSensorState = digitalRead(lightSensorPin);
  if (currentLightSensorState != lightSensorState) {
    lightSensorState = currentLightSensorState;
    if (lightSensorState == HIGH) {
      coverCount++;
      if (coverCount == 1) {
        rotateServo(180);
      } else if (coverCount == 2) {
        rotateServo(0);
        coverCount = 0;
      }
    } else {
      rotateServo(servoAngle);
    }
  }
  /* Đọc dữ liệu của DS1307 */
  readDS1307();
  /* Hiển thị thời gian ra LCD */
  digitalClockDisplay();
  delay(1000);
}
void rotateServo(int angle) {
  servo.write(angle);
  servoAngle = angle;
  delay(500);
}
void readDS1307() {
  Wire.beginTransmission(DS1307);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307, NumberOfFields);
  second = bcd2dec(Wire.read() & 0x7f);
  minute = bcd2dec(Wire.read());
  hour = bcd2dec(Wire.read() & 0x3f);  // chế độ 24h.27
  wday = bcd2dec(Wire.read());
  day = bcd2dec(Wire.read());
  month = bcd2dec(Wire.read());
  year = bcd2dec(Wire.read());
  year += 2000;
}
/* Chuyển từ format BCD (Binary-Coded Decimal) sang Decimal */
int bcd2dec(byte num) {
  return ((num / 16 * 10) + (num % 16));
}
/* Chuyển từ Decimal sang BCD */
int dec2bcd(byte num) {
  return ((num / 10 * 16) + (num % 10));
}
void digitalClockDisplay() {
  // digital clock display of the time
  lcd.clear();          // Xóa màn hình LCD
  lcd.setCursor(0, 0);  // Đặt con trỏ vị trí đầu tiên của hàng đầu tiên
  lcd.print("Time: ");
  lcd.print(hour);
  printDigits(minute);
  printDigits(second);
  lcd.setCursor(0, 1);  // Đặt con trỏ vị trí đầu tiên của hàng thứ hai
  lcd.print("Date: ");
  lcd.print(day);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
}
void printDigits(int digits) {
  // các thành phần thời gian được ngăn chách bằng dấu :
  lcd.print(":");
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

/* cài đặt thời gian cho DS1307 */
void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr) {
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0x00));  // đặt lại pointer
  Wire.write(dec2bcd(sec));
  Wire.write(dec2bcd(min));
  Wire.write(dec2bcd(hr));
  Wire.write(dec2bcd(wd));  // day of week: Sunday = 1, Saturday = 7
  Wire.write(dec2bcd(d));
  Wire.write(dec2bcd(mth));
  Wire.write(dec2bcd(yr));
  Wire.endTransmission();
}