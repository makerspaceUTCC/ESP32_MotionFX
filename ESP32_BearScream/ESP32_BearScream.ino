#include "SoundData2.h"
#include "XT_DAC_Audio.h"
#include <ESP32Servo.h>
#include <esp_task_wdt.h>
Servo myservo;
XT_Wav_Class Sound(rawData);     
XT_Sequence_Class Sequence;   
XT_DAC_Audio_Class DacAudio(25,0);    

uint32_t DemoCounter=0;               
int pos = 0; 
int servoPin = 18;
int play=0;
int sensor=0;
int i,j;

void Robot_Task(void *p){
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  while(1){
    if(sensor==1){
      play=1; digitalWrite(26, HIGH);
      for( i=0;i<3;i++){
        for( j=30;j<=150;j=j+3)
        {myservo.write(j);
        vTaskDelay(20);
        }
       for( j=150;j>=30;j=j-3)
        {myservo.write(j);
        vTaskDelay(20);}
      } sensor=0;
    }
    else {play=0; digitalWrite(26, LOW);}
  vTaskDelay(10);
  esp_task_wdt_reset();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  xTaskCreate(&Robot_Task, "Robot_Task", 2048, NULL, 10, NULL);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 500, 2400); // using SG90 servo min/max of 500us and 2400us
}
unsigned long lastMilli1=millis();
unsigned long lastMilli2=millis();
unsigned long lastMilli3=millis();

int ledState = LOW;
int ledState2 = LOW;

void loop() {

  DacAudio.FillBuffer(); 
 
    if(play==1)
    {
     PlayNumber("0");
    delay(100);
    play=0;
    }
  if (millis()-lastMilli2>400L)
   {   
      sensor=digitalRead(12);
      if (sensor==1){
        digitalWrite(26, HIGH);
      }
      else { digitalWrite(26, LOW);}

      lastMilli2=millis();
   }
// turn the LED 27 on (HIGH is the voltage level)
    if (millis()-lastMilli3>1000L)
   {   
      if (ledState == LOW) {
      ledState = HIGH;
      } else {
      ledState = LOW;
      }
      digitalWrite(27, ledState);
      lastMilli3=millis();
   }
// turn the LED 14 on (HIGH is the voltage level)
  if (millis()-lastMilli1>500L)
  {
    if (ledState2 == LOW) {
      ledState2 = HIGH;
      } else {
      ledState2 = LOW;
      }
      digitalWrite(14, ledState2);
    lastMilli1=millis();
  }

}

void PlayNumber(char const *Number)
{  
  int NumChars=strlen(Number);              // could lose this line of put strlen in loop below, but bad form to do so
  Sequence.RemoveAllPlayItems();            // Clear out any previous playlist
  for(int i=0;i<NumChars;i++){
    AddNumberToSequence(Number[i]);         // For each number add in the sound for that number to the sequence
    Serial.println(Number[i]); 
  }
    DacAudio.DacVolume=100;                 // set volume
    DacAudio.Play(&Sequence);                 // Play the sequence, will not wait here to complete, works independently of your code
}

void AddNumberToSequence(char TheNumber)
{
  switch(TheNumber)
  {
    case '0' : Sequence.AddPlayItem(&Sound);break;
  }
}

