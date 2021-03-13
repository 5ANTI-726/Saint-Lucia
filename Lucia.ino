
#include "IRremote.h"
#include "SR04.h"
#define TRIG_PIN 11
#define ECHO_PIN 10
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long a;
int ledPins = 5;
int buzzer = 9;

int receiver = 2;
IRrecv irrecv(receiver);
decode_results results;

////////////////////////////////////////////////////////////////////////////////////////////////////////
void translateIR(){
  switch(results.value){
    case 0xFFA25D: Serial.println("Beginning bat mode"); bat(); break;
    case 0xFFE21D: Serial.println("Beginning finder mode"); finder(); break;
    case 0xFF02FD: Serial.println("Error diagnostic mode"); break;
    default: break;
  }
}
//////////////////////////////////////////////////
boolean translateIRbat(){
  switch(results.value){
    case 0xFFA25D: return true; break;
    case 0xFF02FD: Serial.println("Error diagnostic mode"); break;
    default: break;
  }
}
//////////////////////////////////////////////////
boolean translateIRfinder(){
  switch(results.value){
    case 0xFFE21D: return true; break;
    case 0xFF02FD: Serial.println("Error diagnostic mode"); break;
    default: break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
   Serial.begin(9600);
   delay(500);
   pinMode(buzzer,OUTPUT);
   irrecv.enableIRIn();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
   if (irrecv.decode(&results)){
     translateIR();
   }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void finder() {
   delay(200);
   irrecv.resume();
   
   unsigned char d;
   
   for(d=0;d<1;){
     digitalWrite(buzzer,HIGH);
     digitalWrite(ledPins, HIGH);
     delay(35);
     
     digitalWrite(buzzer,LOW);
     digitalWrite(ledPins, LOW);
     delay(180);

     if (irrecv.decode(&results)){
       if(translateIRfinder()){
         irrecv.resume();
         d++;
       }
     }
   }
   d = 0;
   return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void bat() {
  delay(200);
  irrecv.resume();
  
  boolean logic = false; unsigned char b; int last = 0;
  
  while(logic == false){
    last = a;
    a = sr04.Distance();
    if(a > 400){
      a = last;
    }
    else{
      a = (a + last*0.1)/1.1;
    }
    
    int buzz = a*0.87+0.25;
    int cycle = round(1400/(1.74*a+0.5));
    
    Serial.print(a);
    Serial.println("cm");
    
    if(a <= 400){
      for(b=0;b<cycle;b++){
        digitalWrite(buzzer,HIGH); digitalWrite(ledPins, HIGH);
        delay(buzz);
        
        digitalWrite(buzzer,LOW);digitalWrite(ledPins, LOW);
        delay(buzz);
        b = b + 1;
      }
    }
    if (irrecv.decode(&results)){
       if(translateIRfinder()){
         irrecv.resume();
         return;
       }
    }
  }
}
