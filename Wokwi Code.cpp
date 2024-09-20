#define BLYNK_TEMPLATE_ID "TMPL3S2dI5AE4"
#define BLYNK_TEMPLATE_NAME "Manish More"
#define BLYNK_AUTH_TOKEN "your auth token"    //connection with blynk

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";  

BlynkTimer timer;

int dirPin[] = {12, 32, 25, 27};
int stepPin[] = {13, 33, 26, 14};
const int stepsPerRevolution = 200;  //stepper pins

int FrontTrigPin = 15;
int FrontEchoPin = 2;
int BackTrigPin = 4;
int BackEchoPin = 16; //dist sensor

long duration;
int distance;

const float BETA = 3950; // Beta Coefficient of the thermo-resistor

// setup and pinmode
void setup()
{
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, temprature);

  pinMode(FrontTrigPin, OUTPUT);
  pinMode(FrontEchoPin, INPUT);
  pinMode(BackTrigPin, OUTPUT);
  pinMode(BackEchoPin, INPUT);

  for (int i = 0; i < 4; i++)
  {
    pinMode(stepPin[i], OUTPUT);
    pinMode(dirPin[i], OUTPUT);
  }
}

//forward function
void forward(){
  digitalWrite(dirPin[0], HIGH);
  digitalWrite(dirPin[1], LOW);
  digitalWrite(dirPin[2], HIGH);
  digitalWrite(dirPin[3], LOW);

  for(int i = 0; i < stepsPerRevolution; i++){
    for(int x = 0; x < 4; x++){
      digitalWrite(stepPin[x], HIGH);
    }
    delayMicroseconds(5000);
    for(int x = 0; x < 4; x++){
      digitalWrite(stepPin[x], LOW);
    }
    delayMicroseconds(5000);
  }
  Serial.println("Moving forward...");
}

//backward function
void backward(){
  digitalWrite(dirPin[0], LOW);
  digitalWrite(dirPin[1], HIGH);
  digitalWrite(dirPin[2], LOW);
  digitalWrite(dirPin[3], HIGH);

  for(int i = 0; i < stepsPerRevolution; i++){
    for(int x = 0; x < 4; x++){
      digitalWrite(stepPin[x], HIGH);
    }
    delayMicroseconds(5000);
    for(int x = 0; x < 4; x++){
      digitalWrite(stepPin[x], LOW);
    }
    delayMicroseconds(5000);
  }
  Serial.println("Moving backward...");
}

//distance sensor
int getDistance(int trigPin, int echoPin){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2; // Convert time into distance
  return distance;
}

//gathering temp data
void temprature(){
  int analogValue = analogRead(34);
  double celsius = 1 / (log(1 / (4095.0 / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
  Serial.print("Temperature: ");
  Serial.print(celsius);
  Serial.println(" ℃");
  delay(1000); // Wait for 1 second before the next reading
  Blynk.virtualWrite(V1, celsius);
}


void loop()
{
  int FrontDistance = getDistance(FrontTrigPin, FrontEchoPin);
  Serial.print("Front Distance: ");
  Serial.println(FrontDistance);

  if(FrontDistance < 50){
    backward();
  } else {
    // Check the back sensor
    int BackDistance = getDistance(BackTrigPin, BackEchoPin);
    Serial.print("Back Distance: ");
    Serial.println(BackDistance);

    if(BackDistance < 50){
      
      forward();
    } else {
      // Default action if no objects are within 50cm
      Serial.println("No obstacles detected, standing still.");
      delay(1000); 
    }
  }

  // Check temperature
  temprature();
  Serial.println("___________________");

  Blynk.run();
  timer.run();
}
