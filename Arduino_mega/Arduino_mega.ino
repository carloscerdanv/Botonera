/*
  Código para placa Arduino Mega 2560 para la asignatura ingenia Diseño de Sistemas Inteligentes con Robots y AGV
  EQUIPO 3 Los Tres3es
  
  Autor: Carlos Cerdán Villalonga
  Fecha de última modificación: 11/05/2022

*/

#include <SPI.h>
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "GFButton.h"
#include <stdio.h>

//Butoon_prueba
// Create two button instances on pins 2 & 3
GFButton b2(2);
GFButton b1(3);

//BASE DATOS
String fallo;
volatile int Num, Lote;

//SP_Arduino2
volatile byte comando = 0;

// SPI_Arduino
char buff = '0';
volatile byte indx;
volatile boolean process;

//LCD_KEYPAD
LiquidCrystal_I2C lcd(0x27,16,2);
const byte FILAS = 4;     // define numero de filas
const byte COLUMNAS = 4;    // define numero de columnas
char keys[FILAS][COLUMNAS] = {    // define la distribucion de teclas
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pinesFilas[FILAS] = {11,10,9,8};   // pines correspondientes a las filas
byte pinesColumnas[COLUMNAS] = {6,7,5,4}; // pines correspondientes a las columnas (7 y 6 girados por la conexión establecida)

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);  // crea objeto

char TECLA;       // almacena la tecla presionada
byte INDICE = 0;      // indice del array
char Error[2];
int i = 0, k = 0;
char NUM = '0' ;

int Estado_prev = 0; 
volatile int Estado = 0;

int PASO = 0;
//****** Setup ***************************************************************************

void setup() {
  Serial.begin(115200);
 // Configurar MISO como salida esclavo (slave out)
  pinMode(MISO, OUTPUT);
 
  // Activa el bus SPI en modo esclavo
  SPCR |= _BV(SPE);
 
  // Activa las interrupciones en el bus SPI
  SPCR |= _BV(SPIE);
  indx = 0; // buffer empty
  process = false;
  SPI.attachInterrupt(); // turn on interrupt

 lcd.init();   //inicializo
 lcd.backlight();  //enciende luz de la pantalla
 lcd.clear();      //Limpio pantalla
 lcd.setCursor (0,0);  //segunda columna seguna fila
 lcd.print("     AGV EN");
 lcd.setCursor (0,1);  //segunda columna seguna fila
 lcd.print("     ESPERA");
 lcd.display();
 pinMode(digitalPinToInterrupt(2), INPUT_PULLUP); 
 pinMode(digitalPinToInterrupt(3), INPUT_PULLUP); 
 pinMode(44, OUTPUT); //Camara
 pinMode(46, OUTPUT);
 pinMode(47, OUTPUT);
 digitalWrite(44, LOW);
 digitalWrite(46, LOW);
 digitalWrite(47, LOW);

 pinMode(36, OUTPUT);
 pinMode(37, OUTPUT);
 
}

// **** Main program ***********************************************

void loop() {
     //Serial.println(Estado);
    if(Estado != 1 && Estado != 2 && Estado != 4){
      digitalWrite(36, HIGH);
      if (b1.wasPressed()) {
        digitalWrite(46, HIGH);
        digitalWrite(36,LOW);
        llamada();
      } 
      }
    if(Estado_prev != Estado){
    switch(Estado){
      case 0: 
        digitalWrite(36, HIGH);
      lcd.clear();      //Limpio pantalla
      lcd.setCursor (0,0);  //segunda columna seguna fila
 lcd.print("     AGV EN");
 lcd.setCursor (0,1);  //segunda columna seguna fila
 lcd.print("     ESPERA");
 lcd.display();
          break;
      case 1: 
        digitalWrite(36,LOW);
        llamada();
        break;
      case 2: 
        digitalWrite(36,LOW);
        CNC();
        break;
      case 3:
        digitalWrite(36, HIGH);
        /*if (b1.wasPressed()) {
          digitalWrite(46, HIGH);
          digitalWrite(36,LOW);
          llamada();
          } */
          break;
      case 4: 
        digitalWrite(36,LOW);
        cobot();
        break;
      case 5:
        digitalWrite(36, HIGH);
        /*if (b1.wasPressed()) {
          digitalWrite(46, HIGH);
          digitalWrite(36,LOW);
          llamada();
          } */
          break;
      case 9:   
           emergencia();
           break;
      default: break;
    }
    }
}

//Funciones*********************************

// Rutina de servicio de interrupciones del bus SPI
ISR (SPI_STC_vect)
{
  byte c = SPDR;
      buff = c; // save data in the next index in the array buff
      process = true;
      Estado_prev = Estado;
      Estado = buff - '0';
      
} 

void llamada(){
    lcd.clear();
    lcd.setCursor (0,0);  //segunda columna seguna fila
    lcd.print("AGV: ME ");
    lcd.setCursor (0,1);  //segunda columna seguna fila
    lcd.print("LLAMASTE WACHO?");
    lcd.display();
    delay(500);
    digitalWrite(46, LOW);
    //do{} while(Estado !=2);
    //CNC();

    return;
}

void envio(){
    lcd.clear();
    lcd.setCursor (0,0);  //segunda columna seguna fila
    lcd.print("AGV ENVIADO");
    lcd.setCursor (0,1);  //segunda columna seguna fila
    lcd.print("AL COBOT");
    lcd.display();
   // do{} while(Estado !=4);
   //   cobot();
    return;
}

void cobot(){
    lcd.clear();
    lcd.setCursor (0,0);  //segunda columna seguna fila
    lcd.print("AGV: CON MI ");
    lcd.setCursor (0,1);  //segunda columna seguna fila
    lcd.print("SOCIO EL COBOT");
    lcd.display();
    do{} while(Estado == 4);
      lcd.clear();        //Limpio pantalla
      lcd.setCursor (0,0);  //segunda columna seguna fila
      lcd.print("AGV:VOY A POR UN");
      lcd.setCursor (0,1);  //segunda columna seguna fila
      lcd.print("ZUMITO DE LITIO");
      lcd.display();
    return;
}

void CNC(){
  lcd.clear();
  lcd.setCursor (0,0);  //segunda columna seguna fila
  lcd.print("1 Intro Error");
  lcd.setCursor (0,1);  //segunda columna seguna fila
  lcd.print("2 Borrar anterior");
  lcd.display();
  digitalWrite(37,HIGH);
  PASO = 0;
  do{
    if(Estado != 2){break;} 
    if(b2.wasPressed()) { PASO = 1;}
    TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable
    if (TECLA == '1'){ PASO = 2;}
    if (TECLA == '2'){ PASO = 3;}
  } while(PASO == 0);
  
  switch(PASO)  {
    case 1:
      Lote++;
      digitalWrite(47, HIGH);
      delay(500);
      digitalWrite(47, LOW);
      digitalWrite(37, LOW);
      envio();
      break;
    case 2:
      digitalWrite(37, LOW);
      intro_datos();
      break;
    case 3:
      digitalWrite(37, LOW);
      borrar_dato();
      break;
    default: break;
  }
   return;
}

void intro_datos(){
  lcd.clear();
  lcd.setCursor (0,0);  //segunda columna seguna fila
  lcd.print("Tipo de error");
  lcd.setCursor (0,1);  //segunda columna seguna fila
  lcd.display();
  while(INDICE < 2){
     if(Estado == 9){emergencia();}
     TECLA = teclado.getKey(); 
     if(TECLA)
     {
        Error[INDICE] = TECLA;
        lcd.setCursor(INDICE,1);
        lcd.print(TECLA);
        lcd.display();
        INDICE++;       // incrementa indice en uno
     }     
  }
    INDICE = 0;
    digitalWrite(44, HIGH);
      delay(10);
    switch((int)Error[0]+(((int)Error[1])*(int)Error[1]))  {
    case 'A'+'1'*'1':
      fallo = "A1";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<1 ;k++){
        digitalWrite(46, HIGH);
        delay(100);
        digitalWrite(46, LOW);
        delay(100);}
      //digitalWrite(44, LOW);
      //delay(10);
      num_error();
      break;
    case 'A'+'2'*'2':
    fallo = "A2";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<2 ;k++){
        digitalWrite(46, HIGH);
        delay(100);
        digitalWrite(46, LOW);
        delay(100);}
      //digitalWrite(44, LOW);
      //delay(10);
      num_error();
      break;
    case 'B'+'1'*'1':
      fallo = "B1";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<3 ;k++){
        digitalWrite(46, HIGH);
        delay(100);
        digitalWrite(46, LOW);
        delay(100);}
      delay(10);
      num_error();
      break;
    case 'B'+'2'*'2':
      fallo = "B2";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<4 ;k++){
        digitalWrite(46, HIGH);
        delay(100);
        digitalWrite(46, LOW);
        delay(100);}
      delay(10);
      num_error();
      break;
    case 'C'+'1'*'1':
      fallo = "C1";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<5;k++){
        digitalWrite(46, HIGH);
        delay(100);
        digitalWrite(46, LOW);
        delay(100);}
      delay(10);
      num_error();
      break;
   /* case 'C'+'2'*'2':
      fallo = "C2";
      digitalWrite(44, HIGH);
      delay(10);
      for(k=0; k<6 ;k++){
        digitalWrite(46, HIGH);
        delay(5);
        digitalWrite(46, LOW);
        delay(5);}
      delay(10);
      digitalWrite(44, LOW);
      num_error();
      break;*/
     case '*'+'#'*'#':
      lcd.clear();
        lcd.setCursor (0,0);  //segunda columna seguna fila
        lcd.print("A POR LA 14");
        lcd.setCursor (0,1);  //segunda columna seguna fila
        lcd.print("Real Madrid <3");
        lcd.display();
        delay(3000);
        intro_datos();
      break; 
    default:  
        lcd.clear();
        lcd.setCursor (0,0);  //segunda columna seguna fila
        lcd.print("Error NO ");
        lcd.setCursor (0,1);  //segunda columna seguna fila
        lcd.print("EXISTENTE");
        lcd.display();
        delay(2000);
        intro_datos();
        break; 
  }
   return;
}

void num_error(){
  lcd.clear();
  lcd.setCursor (0,0);  //segunda columna seguna fila
  lcd.print("Cantidad ");
  lcd.setCursor (0,1);  //segunda columna seguna fila
  lcd.print("Introducida ");
  lcd.display();
  INDICE = 0;
  while(INDICE < 1){
     if(Estado == 9){emergencia();}
     TECLA = teclado.getKey(); 
     if(TECLA)
     {
        if((TECLA - '0')<10 && (TECLA - '0')>0) INDICE++;      
     }
     Num = TECLA - '0';     
  }
    //digitalWrite(44, HIGH);
    //delay(10);
    for(k=0; k<Num ;k++){
      digitalWrite(47, HIGH);
      delay(100);
      digitalWrite(47, LOW);
      delay(100);}
    delay(10);
    digitalWrite(44, LOW);
    INDICE = 0;
    lcd.clear();
    lcd.setCursor (0,0);  //segunda columna seguna fila
    lcd.print("Introduciendo... ");
    lcd.display();
    Serial.print(fallo);
    Serial.print(",");
    Serial.print(Num);
    Serial.print(", N");
    Serial.print(Lote);
    Serial.print(",");
    Serial.println("-");
    delay(1000);
    CNC();

     return;
  
}
void borrar_dato(){
  lcd.clear();
  lcd.setCursor (0,0);  //segunda columna seguna fila
  lcd.print("Borrando ... ");
  lcd.display();
  Serial.print(fallo);
  Serial.print(",");
  Serial.print(Num);
  Serial.print(", N");
  Serial.print(Lote);
  Serial.print(",");
  Serial.println("Borrar");
  delay(5000);
  CNC();

  return;
}

void emergencia(){
  //Serial.println("emergency");
  lcd.clear();
  lcd.setCursor (0,0);  //segunda columna seguna fila
  lcd.print("Auxilio me  ");
  lcd.setCursor (0,1);  //segunda columna seguna fila
  lcd.print("    desmayo ");
  lcd.display();

  while(Estado == 9){
    digitalWrite(37,HIGH);
    digitalWrite(36,HIGH);
    delay(2000);
    digitalWrite(36,LOW);
    digitalWrite(37,LOW);
    delay(1000);
    
  }

    lcd.clear();      //Limpio pantalla
    lcd.setCursor (0,0);  //segunda columna seguna fila
    lcd.print("     AGV EN");
    lcd.setCursor (0,1);  //segunda columna seguna fila
    lcd.print("     ESPERA");
    lcd.display();
  return;
}
