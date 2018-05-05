#define WAITER 50
#define HEATTIME 100000

#define LED_ERROR_COUNTER 25

#define BEEP_STATE_OFF 0
#define BEEP_STATE_RARE 1
#define BEEP_STATE_OFTEN 2

#define BEEP_RARE 25
#define WAIT_RARE 1000
#define BEEP_OFTEN 25
#define WAIT_OFTEN 100

#define LED_ERROR 6

#include <SFE_BMP180.h>
#include <Wire.h>

int state = 1;
int led_error_counter = 0;
int led_error_state = 0;
int beep_state = 0;
int beep_counter = 0;
bool heat = 0;
byte x[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte in[9];
int k = 0;
int waiter;
int value;
String str = "Preheat.";

uint32_t htime;
double t;
double p;

SFE_BMP180 pressure;

byte checkSum(byte *ar){
  byte checksum = 0;
  for(int i = 1; i<8; i++){
    checksum+=ar[i];
  }
  checksum = 0xff-checksum+1;
  return checksum;
}


void setupTimer(){
  OCR4A = 30;
  TIMSK4 |= 0b01000000;
  TCCR4B &= 0b11110000;
  TCCR4B |= 0b00001011;
}

ISR(TIMER4_COMPA_vect){
//  digitalWrite(6, LOW);
//  TIMSK4 &= 0b10111111;
  if(led_error_state == 1){
    led_error_counter--;
    if(led_error_counter<=0){
      led_error_state=0;
      digitalWrite(LED_ERROR, LOW);
    }
  }

  if(beep_state == BEEP_STATE_OFF){
    noTone(13);
  }
  if(beep_state == BEEP_STATE_RARE){
    if(beep_counter <= 0){
      noTone(13);
      beep_counter = BEEP_RARE + WAIT_RARE;
    }
    if(beep_counter == BEEP_RARE) tone(13, 432);
    beep_counter--;
  }
  if(beep_state == BEEP_STATE_OFTEN){
    if(beep_counter <= 0){
      noTone(13);
      beep_counter = BEEP_OFTEN + WAIT_OFTEN;
    }
    if(beep_counter == BEEP_OFTEN) tone(13, 432);
    beep_counter--;
  }
}

void err(){
  digitalWrite(LED_ERROR, HIGH);
  led_error_state = 1;
  led_error_counter = LED_ERROR_COUNTER;
}

void led(int a1, int a2){
  digitalWrite(12, HIGH);
  digitalWrite(10, HIGH); 
  byte b=0;
  if(a1>400){
    digitalWrite(8, HIGH);
    a1-=400;
  }
  for(int i = 0; i<8; i++){
    if(a1>400) {
      b += 1<<i;
      a1-=400;
    }
    else break;
  }
  shiftOut(9, 11, MSBFIRST, b);
  b = 0;
  if(a2>400){
    digitalWrite(7, HIGH);
    a2-=400;
  }
  for(int i = 0; i<8; i++){
    if(a2>400) {
      b += 1<<i;
      a2-=400;
    }
    else break;
  }
  shiftOut(9, 11, MSBFIRST, b);
  
  digitalWrite(10, LOW);
}

double getTemperature()
{
  char status;
  double T;

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      return T;
    }
    else str = "Error retrieving temperature measurement.";
  }
  else str = "Error starting temperature measurement.";
}

double getPressure(double T)
{
  char status;
  double P,p0,a;
  status = pressure.startPressure(3);
  if (status != 0)
  {
    delay(status);
    status = pressure.getPressure(P,T);
    if (status != 0)
    {
      return(P);
    }
    else str = "Error retrieving pressure measurement.";
  }
  else str = "Error starting pressure measurement.";
}





void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  if (!pressure.begin()) return;
  
  setupTimer();

  for(int i = 4; i<=13; i++) pinMode(i, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(12, LOW);
}


void loop() {
  byte tmp;
  
  if(Serial.read()=='=') Serial.println(str);
  
  if(!heat) htime = millis();
  if(htime>HEATTIME&&!heat){
    heat = 1;
    digitalWrite(5, LOW);
    str = "";
  }

  if(value<3000) {
    beep_state = BEEP_STATE_OFF;
  }
  else if(value<4000){
    beep_state = BEEP_STATE_RARE;
  }
  else {
    beep_state = BEEP_STATE_OFTEN;
  }
  
//  if(value>4000&&!buzz&&millis()>buzztime+1000){
//    tone(13, 432);
//    buzz = 1;
//    buzztime = millis();
//  }
//
//  if(millis()>buzztime+20&&buzz){
//    noTone(13);
//    buzz = 0;
//  }
  
  if(state==1&&heat){
    for(int i = 0; i<9; i++){
      Serial1.write(x[i]);
      in[i]=0x00;  
    }
    state = 2;
    waiter = WAITER;
    //Serial.println("Packet Transieved");
  }
  if(state==2){
    //Serial.print("Stage 2 ");
    if(0||Serial1.available()){
      //Serial.print(" Available ");
      waiter = WAITER;
      tmp = Serial1.read();

      if(tmp==0xff){
        k=0;
        in[k]=tmp;
      }
      else{
        k++;
        in[k]=tmp;
        if(k>=8){
          state = 3;
          k = 0;
        }
      }   
    }
    else{
      waiter--;
      if(waiter<=0){
        state = 1;
        k = 0;
        delay(10);
        //Serial.println("BREAK");
        str = "BREAK.";
        err();
      }
    }
  }
  if(state==3){
    if(checkSum(in)==in[8]) {
      value = (int)in[2]*256 + (int)in[3];
      str = "#";
      str += String(value);
      str += " ";
      t = getTemperature();
      str += String(t);
      str += " ";
      p = getPressure(t);
      str += String(p);
      str += '.';
    }
    else {
      //Serial.println("CheckSum failed");
      str = "Checksum failed.";
    }
    //Serial.println(str);  
    led(value, value);
    state = 1;
    delay(50);
  }
}
