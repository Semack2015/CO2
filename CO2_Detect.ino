#define WAIT_TIME 500000

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);
  Serial.begin(9600);
  Serial1.begin(9600);
}

int state = 1;
byte x[] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte in[9];
int k = 0;
int waiter;
 
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

void loop() {
  byte tmp;
  
  if(state==1){
    //delay(100);
    for(int i = 0; i<9; i++){
      Serial1.write(x[i]);
      in[i]=0x00;  
    }
    state = 2;
    waiter = 50;
    Serial.println("Packet Transieved");
  }
  if(state==2){
    //Serial.print("Stage 2 ");
    if(0||Serial1.available()){
      //Serial.print(" Available ");
      waiter = 50;
      tmp = Serial1.read();
      
      if(tmp==0xff){
        //Serial.println("0xff");
        k=0;
        in[k]=tmp;
        Serial.print(tmp);
        Serial.print(' ');
      }
      else{
        k++;
        in[k]=tmp;
        Serial.print(tmp);
        Serial.print(' ');
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
    Serial.println("OK");
    state = 1;
    delay(1000);
  }
}
