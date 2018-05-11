#define WAITER 50
#define HEATTIME 1000

#define LED_ERROR_STATE_OFF 0
#define LED_ERROR_STATE_ON 1

#define LED_ERROR_COUNTER 25

#define BEEP_STATE_OFF 0
#define BEEP_STATE_RARE 1
#define BEEP_STATE_OFTEN 2

#define BEEP_RARE 25
#define WAIT_RARE 1000
#define BEEP_OFTEN 25
#define WAIT_OFTEN 100

#define BEEP_OUTPUT A0
#define LED_ON 4
#define LED_HEAT 5
#define LED_ERROR 6

#define DS_OUTPUT 9
#define LATCH_OUTPUT 10
#define CLOCK_OUTPUT 11
#define MR_OUTPUT 12

#include <SFE_BMP180.h>
#include <Wire.h>

int state = 1;
int led_error_counter;
int led_error_state;
int beep_state;
int beep_counter;
bool heat = 0;
byte x[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte in[9];
int k = 0;
int waiter;
unsigned int value;
String str = "Preheat.";
int bug = 0;
int delay_counter;
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
  if(bug) delay_counter--;
  if(delay_counter<=0) bug = 0;
    
  if(led_error_state == LED_ERROR_STATE_ON){
    led_error_counter--;
    if(led_error_counter<=0){
      led_error_state=LED_ERROR_STATE_OFF;
      digitalWrite(LED_ERROR, LOW);
    }
  }

  if(beep_state == BEEP_STATE_OFF){
    noTone(BEEP_OUTPUT);
  }
  if(beep_state == BEEP_STATE_RARE){
    if(beep_counter == 0){
      noTone(BEEP_OUTPUT);
      beep_counter = BEEP_RARE + WAIT_RARE;
    }
    if(beep_counter == BEEP_RARE) tone(BEEP_OUTPUT, 432);
    beep_counter--;
  }
  if(beep_state == BEEP_STATE_OFTEN){
    if(beep_counter == 0){
      noTone(BEEP_OUTPUT);
      beep_counter = BEEP_OFTEN + WAIT_OFTEN;
    }
    if(beep_counter == BEEP_OFTEN) tone(BEEP_OUTPUT, 432);
    beep_counter--;
  }
}

void err(){
  digitalWrite(LED_ERROR, HIGH);
  led_error_state = LED_ERROR_STATE_ON;
  led_error_counter = LED_ERROR_COUNTER;
}

void led(int a1, int a2){
  digitalWrite(MR_OUTPUT, HIGH);
  digitalWrite(LATCH_OUTPUT, HIGH); 
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
  shiftOut(DS_OUTPUT, CLOCK_OUTPUT, MSBFIRST, b);
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
  shiftOut(DS_OUTPUT, CLOCK_OUTPUT, MSBFIRST, b);
  
  digitalWrite(LATCH_OUTPUT, LOW);
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

  pinMode(LED_ON, OUTPUT);
  pinMode(LED_HEAT, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(DS_OUTPUT, OUTPUT);
  pinMode(LATCH_OUTPUT, OUTPUT);
  pinMode(CLOCK_OUTPUT, OUTPUT);
  pinMode(LATCH_OUTPUT, OUTPUT);
  pinMode(MR_OUTPUT, OUTPUT);
  pinMode(BEEP_OUTPUT, OUTPUT);
  
  digitalWrite(LED_ON, HIGH);
  digitalWrite(LED_HEAT, HIGH);
  digitalWrite(MR_OUTPUT, LOW);
}


void loop() {
  byte tmp;
  
  if(Serial.read()=='=') Serial.println(str);
  
  if(!heat) htime = millis();
  if(htime>HEATTIME&&!heat){
    heat = 1;
    digitalWrite(LED_HEAT, LOW);
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
  
  if(state==1&&heat&&!bug){
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
    if(Serial1.available()){
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
        //Serial.println("BREAK");
        str = "BREAK.";
        err();
        delay(10);
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
      err();
    }
    if(value==0){
      delay_counter = 1000;
      bug = 1;
      str = "BUG.";
    }
    else if(value>0&&value<=5000) 
      led(value, value);
    else str = "Error.";
    state = 1;
    delay(50);
  }
}
