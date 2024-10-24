/*
* Name: clock and temp project
* Author: Leo Dahl
* Date: 2024-10-24
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
  #include <RTClib.h>
  #include "LedControl.h"

  #include <Wire.h>
  #include <Servo.h>
  #include "U8glib.h"
  // Init constants
  const int tempPin = A0;
  const int ServoPin = 9;
  const int ButtonPin = 5;
  const int StartPin = 4;
  const int SoundPin = 3;



  // Init global variables
  
  // construct objects
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
  LedControl lc=LedControl(12,11,10,1);
  Servo myServo;
  RTC_DS3231 rtc;

  unsigned long delaytime=500;
  int Timer = 0;
  int NumTime = 0;

void setup() {
  // Attach objects

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  
  myServo.attach(ServoPin);

  // init communication
  Serial.begin(9600);
  Wire.begin();
  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  u8g.setFont(u8g_font_unifont);
  u8g.firstPage(); 

  pinMode(tempPin,INPUT);
  pinMode(SoundPin, OUTPUT);
}

void loop() {

  float buttonRead = digitalRead(ButtonPin);
  float startRead = digitalRead(StartPin);

  if(buttonRead == 1)
  {
    Timer = Timer + 10;
    AddTime(Timer);
  }
  if(startRead == 1)
  {
    Serial.println("Start");
    Countdown(Timer);
  }

  Serial.println("Button is " + String(buttonRead));

  oledWrite(String(getTime()));
  servoWrite(measureTemp());
  
  
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  Serial.print("Time");
  Serial.println(" " +String(now.hour()) + " " + String(now.minute()) + " " + String(now.second()));
  return (" " +String(now.hour()) + " " + String(now.minute()) + " " + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temperature as float
*/
float measureTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  //steinhart-hart coeficients for thermistor

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  //calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;                                           //convert Kelvin to Celcius

  return temp;
}


/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();
  do {

    u8g.drawStr(0, 22, text.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  Serial.println(value);
  Serial.println(map(value,5,15,0,180));
  myServo.write(map(value,5,15,0,180));
}

void AddTime(int Time)
{
  int ones = Time%10;
  int tens = (Time/10)%10;
  int hundreds = (Time/100)%10;
  int thousands = (Time/1000)%10;
  lc.setDigit(0,3,thousands,false);
  lc.setDigit(0,2,hundreds,false);
  lc.setDigit(0,1,tens,false);
  lc.setDigit(0,0,ones,false);
}

/*
* Takes a time value and counts down and adds it onto the 7 segment display
*Parameters: -value: Time
*Returns: void
*/
void Countdown(int Time)
{
  Serial.println("Added");
  for(int i=Time; i>=0; i--)
  {
    AddTime(i);
    delay(1000);
  }
  Timer = 0;
  Peep();
}

void Peep()
{
  
  for(int i=3; i>=0; i--)
  {
    digitalWrite(SoundPin, HIGH);
    delay(300);
    digitalWrite(SoundPin, LOW);
    delay(300);
  }
}