#define WAITER 50

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  Serial.begin(9600);
  Serial1.begin(9600);
  for(int i = 3; i<=11; i++)
    pinMode(i, OUTPUT);
}

int state = 1;
bool heat = 0;
byte x[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte in[9];
int k = 0;
int waiter;
int value;
String str = "Preheat";
 
//int ansAvailable(void){
//  int cnt = WAIT_TIME;
//  bool t = 0;
//  while(cnt>0){
//    if(Serial1.available()){
//      t=1;
//      break;
//    }
//    cnt--;
//  }
//  if(t) return 1;
//  else{
//    Serial.println("BREAK");
//    return 0;
//  }
//}

byte checkSum(byte *ar){
  byte checksum = 0;
  for(int i = 1; i<8; i++){
    checksum+=ar[i];
  }
  checksum = 0xff-checksum+1;
  return checksum;
}

void Led(int c){
  for(int i = 3; i<=11; i++) digitalWrite(i, LOW); 
  int z = 3;
  while(c>200){
    digitalWrite(z, HIGH);
    z++;
    if(z>11) break;
    c-=450;
  }
}

uint32_t wtime;

void loop() {
  byte tmp;

  if(!heat) wtime = millis();
  if(wtime>100000&&!heat){
    heat = 1;
    str = "Preheat";
  }
  
  if(Serial.read()=='=') Serial.println(str);
  
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
        //Serial.println("0xff");
        k=0;
        in[k]=tmp;
        //Serial.print(tmp);
        //Serial.print(' ');
      }
      else{
        k++;
        in[k]=tmp;
        //Serial.print(tmp);
        //Serial.print(' ');
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
        // Serial.println("BREAK");
        str = "BREAK";
      }
    }
  }
  if(state==3){
    if(checkSum(in)==in[8]) {
      value = (int)in[2]*256 + (int)in[3];
      str = String(value);
    }
    else {
      //Serial.println("CheckSum failed");
      str = "Checksum failed";
    }
    Led(value);
    state = 1;
    delay(50);
  }
}
