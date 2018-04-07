#define WAITER 50

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

int state = 1;
bool heat = 0;
byte x[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte in[9];
int k = 0;
int waiter;
int value;
 
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

void loop() {
  byte tmp;

  if(millis()>100000) heat = 1;
  else Serial.println("Preheat");
  
  if(Serial.read()=='=') Serial.println(value);
  
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
        Serial.println("BREAK");
      }
    }
  }
  if(state==3){
    if(checkSum(in)==in[8]) {
//      Serial.print(in[2]);
//      Serial.print(' ');
//      Serial.println(in[3]);
//      Serial.print('_');
//      Serial.println((int)in[2]*256 + (int)in[3]);
      value = (int)in[2]*256 + (int)in[3];
      
    }
    else Serial.println("CheckSum failed");
    Led(value);
    state = 1;
    delay(50);
  }
}
