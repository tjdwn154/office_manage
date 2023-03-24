#include "SoftwareSerial.h"
#include "WiFiEsp.h"

const byte RELAY = 8;
const byte PIR = 6;
const byte BUTTON = 2;  //BUTTON 인터럽트 핀

byte Flag = 0;

volatile byte ramp = LOW;  //인터럽트 기능 변수, 램프의 ON/OFF 값


unsigned long Time_count = 0;   //모션 센서 사용을 위한 변수 millis()
unsigned long Time_count2 = 0;   //wifi 통신을 위한 변수 millis()


SoftwareSerial Serial1(4, 5); // RX, TX

char ssid[] = "wifi ID"; // network SSID (name)

char pass[] = "wifi PASSWORD"; // network password

int status = WL_IDLE_STATUS; // 

IPAddress serverIP(server ip); 

WiFiEspClient client;

char strBuf[255];

void setup() {
  
  pinMode(RELAY, OUTPUT); //디지털 포트를 OUTPUT으로 설정
  pinMode(PIR, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);  //인터럽트 사용을 위한 INPUT_PULLUP

  attachInterrupt(digitalPinToInterrupt(BUTTON), BT_ISR, CHANGE); //Interrupt Settings
  
  Serial.begin(9600);

  // ESP 모듈용 직렬 초기화

  Serial1.begin(9600);

  // ESP 모듈 초기화

  WiFi.init(&Serial1);
  // WiFi 네트워크에 연결 시도

  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to WPA SSID: ");

    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

  }

  Serial.println("You're connected to the network");

  client.connect(serverIP,9090);

}

void loop() {
  int BUTTON_Value = digitalRead(BUTTON);  //입력 핀의 값을 읽고 변수에 저장
  int PIR_Value = digitalRead(PIR);

  digitalWrite(RELAY, ramp);
  
  //Motion Sensing Part
  if(ramp){
    if(PIR_Value == LOW){  
      
      if(Time_count == 0){    
        Time_count = millis();
      }
      else{       
        
       // Serial.print("No movement(ms) : ");
        //Serial.println(millis() - Time_count);

        if(millis() - Time_count > 10000){    //움직임을 감지한지 10초가 지났을시
          Serial.println("No movement, turn off the LED.");
          Serial.println("");
          digitalWrite(RELAY, LOW);
          ramp = LOW;
        
        Time_count = 0;
        }
        
      }
      
    }
    else{
      Time_count = 0;
    }

  }
  else{
    Time_count = 0;
  }

  //소켓 통신
  if(client.available()){
    memset(strBuf,0x00,sizeof(strBuf));
    int recv=client.read(strBuf,255);
    Serial.write(strBuf);
    Serial.println("");
  }

  if(Time_count2 == 0){    // n초마다 값 전달
        Time_count2 = millis();
        if(ramp){
          client.write("201YES");
        }
        else{
          client.write("201NO");
        }
  }
  else{
    if(millis() - Time_count2 > 10000){    // 10초
      Time_count2 = 0;
    }
  }


  
//  if(Serial.available()){
//    String recvStr=Serial.readString();
//    client.write((char*)recvStr.c_str(),recvStr.length());
////    client.write(Serial.read());


//  }

}


void BT_ISR(){    //버튼 인터럽트 발생 시 실행되는 함수
  if(digitalRead(BUTTON)==LOW){
    if(Flag==0){  //Run if Flag is 0
      ramp = !ramp;

      Flag = 1;
  
      if(ramp){
        Serial.println("LED ON");
        Serial.println("");
      }
      else{
        Serial.println("LED OFF");
        Serial.println("");
      }
      delay_(200);
    }  
    else{
      //No response.
    }
  }
  else{ //스위치를 누르지 않으면 플래그를 다시 0으로 설정
    Flag=0;
    //Serial.println("떼졌다");
  }

}

unsigned long count = 0;
void delay_(int ms){  //인터럽트 내 지연 사용 기능 추가

  while(count!=ms){
    delayMicroseconds(1000);
    count++;
  }
  count=0; 
}
