/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.
*/

#include <SPI.h>
#include <SD.h>

int DATO_CONTADOR;
int CONTADOR;

bool FLAG;
bool FLAG4;

File myFile;

void setup() {

  Serial.begin(9600);

  // TIMER 1 //
  // TIMER POR DESBORDAMIENTO CON PRESCALER 1024 //
  // TIENE UN PERIODO DE 1S FRECUENCIA DE 1 HZ MARCA EL INICIO DE LA SEÑAL //

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8; //F9E5 100 MS

  TIMSK1 |= (1 << TOIE1); //timer open interrupt enable
  //********************************* TIMER 1 ********************************//

  Serial.print("INICIANDO SD ...");

  if (!SD.begin(4)) {
    Serial.println("ALGO FALLO!");
    while (1);
  }
  Serial.println("TODO JOYA");

  SD.remove("PRUEBA.txt");
  
  DATO_CONTADOR = 0;
  CONTADOR = 0;

  //pinMode(13,OUTPUT);

  myFile = SD.open("PRUEBA.txt", FILE_WRITE);
  if (myFile)
  {
    Serial.println("PRESIONE + PARA INICIAR");
    myFile.println("PRESIONE + PARA INICIAR");
    myFile.close();
  }
  else
  {
    Serial.println("ERROR ABRIENDO PRUEBA.txt");
  }
}

void loop()
{
  SERIAL_EVENT();

  if (CONTADOR >= 5)
  {
    FLAG = 1;
  }

  if (FLAG)
  {
    //digitalWrite(13,1);
    myFile = SD.open("PRUEBA.txt", FILE_WRITE);
    if (myFile)
    {
      Serial.print("CONTADOR: ");
      Serial.println(DATO_CONTADOR);
      myFile.print("CONTADOR: ");
      myFile.println(DATO_CONTADOR);
      myFile.close();
      CONTADOR = 0;
      FLAG = 0;
    }
    //digitalWrite(13,0);
  }
}

  void SERIAL_EVENT()
  {
    while (Serial.available()) // lectura del teclado
    {
      int ESTADO = '0'; // 0 ASCII

      ESTADO = Serial.read(); // La variable estado pasara a valer lo que detecte del teclado

      if (ESTADO == '+') // cuando presione + iniciara la medición
      {
        myFile = SD.open("PRUEBA.txt", FILE_WRITE);
        if (myFile)
        {
          myFile.println ("INICIO");
          Serial.println ("INICIO");
          myFile.close();
          FLAG4 = 1;
        }
      }
      if (ESTADO == '-')
      {
        myFile = SD.open("PRUEBA.txt", FILE_WRITE);
        if (myFile)
        {
          myFile.println ("PAUSA");
          Serial.println ("PAUSA");
          myFile.close();
          FLAG4 = 0;
        }
      }
    }
  }
  //********************************* TIMER 1 *******************************//
  // POR DESBORDAMIENTO //
  // TIMER MADRE DE 1s  //
  // EL FLAG2 SE SETEA EN UNO CADA VEZ QUE INTERRUMPE EL TIMER //
  // LA VARIABLE SEGUNDOS_LCD AUMENTA EN 1 CADA VES QUE INTERRUMPE EL TIMER //
  ISR(TIMER1_OVF_vect)
  {
    TCNT1 = 0xC2F7; // F9E5 100 ms

    if (FLAG4)
    {
      CONTADOR ++ ;
      DATO_CONTADOR ++ ;
    }
  }
  //************************************************************************//
