// library servo
#include <ESP32Servo.h> 

// library timer
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define BLYNK_TEMPLATE_ID           "TMPLCXsnnCju"
#define BLYNK_DEVICE_NAME           "elink feeder"
#define BLYNK_AUTH_TOKEN            "OQ2Akj8O_PPB-io6GeC5dyFG6m8kISed"

// ultrasonic
const int trigPin = 32;
const int echoPin = 33;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;

// voltage sensor 
const int voltageSensor = 15;

float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int ValueVolt = 0;

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// servo init
Servo myservo;
Servo myservo2;

int servoPin = 18;
int servoPin2 = 19;

int state = 0;
int state2 = 0;
int state3 = 0;
int statetimerrun = 0;

float Height = 30.0;

int valueHeight = 0;

// timer 

int Hour = 0;
int HourChangeState3 = 0;

int HourPagi = 9;
int HourTghhari = 13;
int HourPetang = 18;
int HourMalam = 23;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HP";
char pass[] = "nafza9494";

BlynkTimer timer;

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // data masuk off 0 and on 1

  if (pinValue == 1 && state == 0){
    myservo.write(90);
    delay(2000);
    myservo.write(0);
    myservo2.write(90);
    delay(2000);
    myservo2.write(0);
    state = 1;
  }
  else {
    state = 0;
    }

}

void myTimerEvent(){
  Timer();
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;

  valueHeight = Height - distanceCm;

  if (valueHeight <= 3 && state2 == 0){
    Blynk.logEvent("notification", "makanan xde........");
    state2 = 1;
    }
  else {
    state2 = 0;
    }

 ValueVolt = analogRead(voltageSensor);
  vOUT = (ValueVolt * 5.0) / 4095.0;
  vIN = vOUT / (R2/(R1+R2));

  if (vIN >= 4 ){
    statetimerrun = 0;
    }
  else{
    statetimerrun = 1;
    }
  
  }


void Timer(){
  tmElements_t tm;
  RTC.read(tm);

  Hour = tm.Hour;

      switch (statetimerrun) {
      case 1:
        if (Hour == HourPagi || Hour == HourTghhari || Hour == HourPetang || Hour == HourMalam && state3 == 0){
          HourChangeState3 = tm.Hour + 1; 
          myservo.write(90);
          delay(2000);
          myservo.write(0);
          myservo2.write(90);
          delay(2000);
          myservo2.write(0);
          state3 = 1;
          }

        if (HourChangeState3 == 24){
          HourChangeState3 = 0;
          }

       if (state3 == 1){
          if (Hour == HourChangeState3){
            state3 = 0;
            HourChangeState3 = 0;
            }
        }
        break;
      }
  }

void setup()
{
  // Debug console
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  myservo2.setPeriodHertz(50);
  myservo2.attach(servoPin2, 500, 2400);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  myservo.write(0);
  myservo2.write(0);

  

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
}
