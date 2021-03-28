#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "wifi.h"
#include "Timer.h"
Timer timer;
Timer timer1;
#define motor 10
unsigned char mealPlanValue[5];
unsigned long manual_feed;
unsigned char currentTime[8];
unsigned char planTime[5];//only one
SoftwareSerial mySerial(2, 3); // RX, TX
void setup() {
  // put your setup code here, to run once:
  pinMode(motor, OUTPUT);
  digitalWrite(motor, LOW);
  Serial.begin(9600);
  mySerial.begin(9600);    
  wifi_protocol_init();
  //
  timer.every(1000, TIME);
  
  //config network
  timer.after(3000,AFTER);
  byte v = EEPROM.read(0);
  mySerial.println(v);
  EEPROM.write(0,++v);
  if(v>2)
  {
    mySerial.println("config net");
    mcu_reset_wifi();
    mcu_set_wifi_mode(SMART_CONFIG);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  wifi_uart_service();
  serialEvent();
  timer.update();
  timer1.update();
}

void serialEvent() {
 if (Serial.available()) {
 unsigned char ch = (unsigned char)Serial.read();
 //Serial.print(ch);
 uart_receive_input(ch);
 }
}
void TIME()
{
  mcu_get_system_time();
  
  if(currentTime[0]==1)
  {
    mySerial.println("currentTime:");
    for(int i =0;i<8;i++)
    {
      mySerial.print(currentTime[i]);
      mySerial.print(" ");
    }    
    mySerial.println();
    
    if(planTime[4]==1)
    {
      mySerial.println("planTime:");
      for(int i =0;i<5;i++)
      {
        mySerial.print(planTime[i]);
        mySerial.print(" ");
      }
      mySerial.println();
      
      if(planTime[0]==0||planTime[0]==currentTime[7])
      {
        if(planTime[1]==currentTime[4]&&planTime[2]==currentTime[5])
        {  
          planTime[2]-=1; 
          mySerial.println(planTime[2]); 
          timer1.after(60000,BACK);     
          timer.pulse(10,planTime[3]*2*1000,LOW);                    
        }
      }
    }else
    {
        mySerial.println("plan not available");
        mySerial.println();
    }
  }
}
void AFTER()
{
  EEPROM.write(0,0);  
}
void BACK()
{  
  planTime[2]+=1;
  mySerial.println(planTime[2]);  
}
