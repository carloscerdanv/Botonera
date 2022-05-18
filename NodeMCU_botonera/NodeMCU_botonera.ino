/*
  Código para placa NODE MCU ESP8266 para la asignatura ingenia Diseño de Sistemas Inteligentes con Robots y AGV
  EQUIPO 3 Los Tres3es
  
  Autor: Carlos Cerdán Villalonga
  Fecha de última modificación: 11/05/2022

*/

#include "SPI.h"
#include "GFButton.h"

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>


#define int_pin0 D0
#define int_pin1 D1
#define int_pin2 D2
#define int_pin3 D3
#define int_pin4 D4

#define seguir_mision 0
#define pausar_mision 1
#define cancel_mision 2
#define borrar_mision 3

#define TRIGGER 1009
#define TRIGGER2 1010
#define TRIGGER3 1008

#define LED D10

//void ICACHE_RAM_ATTR ISR_em();

volatile char d = '9', c= '0';
char c_prev = '0';
int i =0;
bool aux= HIGH, aux2 = HIGH;
bool rearm = HIGH;
volatile int contador = 0, contador_Num = 0;
bool dato = LOW;
bool datoAnterior = LOW;
bool dato2 = LOW;
bool datoAnterior2 = LOW;
bool PIN = LOW;

GFButton b4(int_pin4);

const int REG = 1071;               // registro 36 Lectura estado posicion MIR
const int Palo1 = 1073;               // registro 37
const int Palo2 = 1075;               // registro 38
const int Palo3 = 1077;               // registro 39
const int Palo4 = 1079;               // registro 40
const int Palo5 = 1081;               // registro 41
IPAddress remote(192, 168, 0, 29);  // Address of Modbus Slave device

uint16_t res = 0;
const uint16_t llam = 1;
const uint16_t env = 3;
volatile bool p = LOW;
ModbusIP mb;  //ModbusIP object

void setup() {
  WiFi.begin("SMARTMAI", "unidad_fabri");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.begin(9600);
  mb.client();
  
   digitalWrite(SS, HIGH); // disable Slave Select
   SPI.begin();

   pinMode(LED, OUTPUT);
   pinMode(digitalPinToInterrupt(int_pin0), INPUT);
   pinMode(digitalPinToInterrupt(int_pin1), INPUT);
   pinMode(digitalPinToInterrupt(int_pin2), INPUT);
   pinMode(digitalPinToInterrupt(int_pin3), INPUT);
   pinMode(digitalPinToInterrupt(int_pin4), INPUT_PULLUP);
   //attachInterrupt(digitalPinToInterrupt(int_pin3), ISR_em, FALLING);

    if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
      mb.writeHreg(remote, REG, res);
      mb.task();
      delay(10);
    }else {
      mb.connect(remote);
    }
    digitalWrite(LED_BUILTIN, LOW);
}
 
void loop() {
 
   if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
    mb.readHreg(remote, REG, &res);
    
    } else {
      mb.connect(remote);
    }
   mb.task();
   delay(50);
   //Serial.println(res); 
   //Serial.println(digitalRead(int_pin4));
   c = '0' + res;
  // if(c_prev != c){
  if(i==100){
      digitalWrite(SS, LOW); // enable Slave Select
      SPI.transfer(c);
      digitalWrite(SS, HIGH);
      delay(50);
      i=0;
  } i++; 
   //c_prev = c;
   
   if(digitalRead(int_pin1)) {  //LLAMADA DE AGV AL CNC
      if(aux){
        llamada();
        aux = LOW;
      }  
   } else{
      aux = HIGH;
   }
   if(digitalRead(int_pin0)) { //ENVIO AGV AL COBOT
    if(aux2){
        envio();
        aux2 = LOW;
      }  
   } else{
      aux2 = HIGH;
   }
   
   
   if(digitalRead(int_pin3)){ 
      ISR_em();
      p = HIGH;  
     }
   
   if(digitalRead(int_pin2)){ 
    PIN = HIGH;
    camara();
    
   }
   
}



void llamada(){

  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
  //mb.writeHreg(remote, REG, llam);
   mb.task();
   delay(10);
   mb.writeCoil (remote, cancel_mision, HIGH);
   mb.task();
   delay(10);
   mb.writeCoil (remote, borrar_mision, HIGH);  
   mb.task();
   delay(10);
   mb.writeCoil(remote, TRIGGER, HIGH);
   mb.task();
   delay(10); 
   mb.writeCoil(remote, seguir_mision, HIGH);
   mb.task();
   delay(10);
   }    
   return; 
}

void envio(){

  digitalWrite(LED_BUILTIN,HIGH);
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
   mb.writeHreg(remote, REG, env);
   delay(10);
   mb.writeCoil (remote, cancel_mision, HIGH);
   delay(10);
   mb.writeCoil (remote, borrar_mision, HIGH);
   delay(10);
   mb.writeCoil (remote, TRIGGER2, HIGH);
   delay(10);
   mb.writeCoil(remote, seguir_mision, HIGH);
   delay(10);
   mb.task();
   delay(10);
   } 
   return;
}

void ISR_em(){
  c= '9';
  Serial.println("ENTRAA");
   digitalWrite(SS, LOW); // enable Slave Select
   SPI.transfer(c);
   digitalWrite(SS, HIGH);
   delayMicroseconds(50);
  if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
   mb.writeCoil(remote, pausar_mision, HIGH);
   Serial.println("EMERGENCIAA");
  }
   mb.task();
   delayMicroseconds(50);
   
   while(p){
    digitalWrite(LED_BUILTIN, HIGH);
     Serial.println("A");
     p= digitalRead(int_pin3);
    }
    Serial.println("sale");
   while(rearm){
    Serial.println("B");
    digitalWrite(LED, HIGH);
    if (b4.wasPressed()){
      rearm = LOW;
      digitalWrite(LED, LOW);
      }
    }
     Serial.println("SALEEEE");
     if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
       mb.writeCoil(remote, seguir_mision, HIGH);
       mb.task();
      delayMicroseconds(50);
      }
     rearm = HIGH;
    
    return;
}

void camara(){

  while(PIN) {
      dato = digitalRead(int_pin1);
      if (dato == HIGH && datoAnterior == LOW)
        {
        contador++;
        }
      datoAnterior = dato;
      dato2 = digitalRead(int_pin0);
      if (dato2 == HIGH && datoAnterior2 == LOW)
        {
        contador_Num++;
        }
      datoAnterior2 = dato2;
      PIN = digitalRead(int_pin2);
    }
    
   if (mb.isConnected(remote)) {   // Check if connection to Modbus Slave is established
      switch(contador){
      case 1:
        mb.writeHreg(remote, Palo1, contador_Num);
        mb.task();
        delay(10);
       break;
      case 2:
        mb.writeHreg(remote, Palo2, contador_Num);
        mb.task();
        delay(10);
       break;
      case 3:
        mb.writeHreg(remote, Palo3, contador_Num);
        mb.task();
        delay(10);
       break;
      case 4:
        mb.writeHreg(remote, Palo4, contador_Num);
        mb.task();
        delay(10);
       break;
      case 5:
        mb.writeHreg(remote, Palo5, contador_Num);
        mb.task();
        delay(10);
       break;
      default:
      break;
      }
    }else {
      mb.connect(remote);
    } 
   contador = 0;
   contador_Num = 0;
   PIN = HIGH;
}
