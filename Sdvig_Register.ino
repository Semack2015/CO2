#define CLOCK 4
#define data 2
#define latch 3

void setup() {
  pinMode(CLOCK, OUTPUT);
  pinMode(data, OUTPUT);
  pinMode(latch, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(latch, HIGH);
  Serial.begin(9600);
}



void Led(int a1, int a2){
  byte b;
  int i = 7;
  int j = 7;
  
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(latch, LOW);
  while(a1>450){
    b += (1<<i);
    //Serial.println(b, BIN);
    a1-=200;
    i--;
    if(i<0){
      digitalWrite(6, HIGH);
      break;
    }
  }
  shiftOut(data, CLOCK, LSBFIRST, b);
  
  b=0;
  while(a2>450){
    b += (1<<j);
    //Serial.println(b, BIN);
    a2-=200;
    j--;
    if(j<0){
      digitalWrite(5, HIGH);
      break;
    }
  }
  shiftOut(data, CLOCK, LSBFIRST, b);
  digitalWrite(latch, HIGH); 
}


void loop() {
    Led(random(400, 2400), random(400, 2400));
    delay(150);
}
