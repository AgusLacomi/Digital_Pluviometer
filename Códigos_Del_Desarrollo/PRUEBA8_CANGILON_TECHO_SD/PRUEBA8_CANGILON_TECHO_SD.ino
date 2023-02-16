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


//********************** Liberias **************************************//
//#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
//********************** Liberias **************************************//

File myFile;

//********************* definicion de variables ************************//
volatile float CONTADOR = 0;
volatile int CONT = 0;
volatile float CONT_LCD = 0;
float CTE;

unsigned long LIMITE = 10;
unsigned long INICIO_TIMER = 0;

int SEGUNDOS_LCD = 0;
int MINUTOS_LCD = 0;
int HORAS_LCD = 0;
int DIAS_LCD = 0;

int SEGUNDOS = 0;
int MINUTOS = 0;
int HORAS = 0;
int DIAS = 0;

int TIEMPO = 0;
int TIEMPO2 = 0;
int TIEMPO3 = 0;

int DIFERENCIA = 0;
int DIFERENCIA2 = 0;
int DIFERENCIA3 = 0;

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;
bool FLAG4 = 0;
//********************* definicion de variables ************************//


//********* ATENCION EEPROM ************//
//********float PROMEDIO = 5.5; ******//
float EEPROMEDIO;
//********* ATENCION EEPROM ************//


//***************************************** SETUP *********************************************//

void setup()
{
  Serial.begin(9600);

  // TIMER 1 //
  // TIMER POR DESBORDAMIENTO CON PRESCALER 1024 //
  // TIENE UN PERIODO DE 1S FRECUENCIA DE 1 HZ MARCA EL INICIO DE LA SEÑAL //

  CONTADOR = 0;
  CONT_LCD = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8; //F9E5 100 MS

  TIMSK1 |= (1 << TOIE1); //timer open interrupt enable
  //********************************* TIMER 1 ********************************//


  //******************** Grabo en EEPROM el valor promedio *******************//

  // EEPROM.put (EEPROMEDIO, PROMEDIO);

  //******************** Grabo en EEPROM el valor promedio *******************//


  //****************************** Inicializacion puerto serie *******************************//

  //****************************** Inicializacion puerto serie *******************************//


  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW);

  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//

  //******************************* inicializacion de variables ******************************//
  FLAG = false;
  FLAG2 = false;
  FLAG3 = true;
  FLAG4 = false;

  CTE = 7.90;
  //******************************* inicializacion de variables ******************************//

  Serial.print("INICIANDO SD ...");

  if (!SD.begin(4)) {
    Serial.println("ALGO FALLO!");
    while (1);
  }
  Serial.println("TODO JOYA");

  SD.remove("PRUEBA.txt");

  pinMode(13, OUTPUT);

  //************************ INICIALIZACION DE PINES DEL LCD ***********************//

  myFile = SD.open("PRUEBA.txt", FILE_WRITE);
  if (myFile)
  {
    Serial.println("PRESIONE + PARA INICIAR");
    Serial.println("PRESIONE - PARA PAUSAR");
    Serial.println("PRESIONE * PARA REINICIAR");
    Serial.println();

    myFile.println("PRESIONE + PARA INICIAR");
    myFile.println("PRESIONE - PARA PAUSAR");
    myFile.println("PRESIONE * PARA REINICIAR");
    myFile.println();
    myFile.close();
  }
  else
  {
    Serial.println("ERROR ABRIENDO PRUEBA.txt");
  }
}

//**********************************************************************************************//


//********************************************** VOID LOOP ****************************************//
void loop()
{
  SERIAL_EVENT();

  //************************ SETEO DE MINUTOS ************************************//
  if (SEGUNDOS_LCD == 60) // SI ALCANZA LOS 60 SEGUNDOS AUMENTA EN 1 EL MINUTO
  {
    MINUTOS_LCD ++; //aumento en 1 la cantidad de minutos
    SEGUNDOS_LCD = 0; // resetea la variable segundos
  }
  //************************ SETEO DE MINUTOS ************************************//


  //************************* SETEO DE HORAS *************************************//
  if (MINUTOS_LCD == 60) // SI ALCANZA LOS 60 MINUTOS AUMENTA EN 1 LA HORA
  {
    HORAS_LCD ++; //aumento en 1 la cantidad de horas
    MINUTOS_LCD = 0; // reseteo la variable minutos
  }
  //************************* SETEO DE HORAS *************************************//


  //************************ RESETEO DE HORAS ************************************//
  if (HORAS_LCD > 23) // SI ALCANZA UN VALOR MAYOR A 24 RESETEA LA VARIABLE HORAS Y RESETEA EL DISPLAY
  {
    DIAS_LCD ++;
    HORAS_LCD = 0; // reseteo la variable en horas
  }
  //************************ RESETEO DE HORAS ************************************//


  //*********************** FLAG ( INTERRUPCIÓN POR FLANCO ) **************************//
  if (FLAG4)
  {
    if (FLAG == 1) // FLAG del servicio de interrupción
    {
      FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción
      CONTADOR ++; // AUMENTA EN 1 EL CONTADOR GENERAL, DE ESTE SE BASAN TODOS LOS DEMAS
      CONT_LCD = CONTADOR * 7.9; // SE MULTIPLICA POR LA VARIABLE DE MEDIDA DE 1 CANGILON 7.9CM3

      DIAS = DIAS_LCD;
      HORAS = HORAS_LCD;
      MINUTOS = MINUTOS_LCD;
      SEGUNDOS = SEGUNDOS_LCD;

      DIFERENCIA3 = HORAS_LCD - TIEMPO3;
      DIFERENCIA2 = MINUTOS_LCD - TIEMPO2;
      DIFERENCIA = SEGUNDOS_LCD - TIEMPO; // ESTE ES EL CALCULO ENTRE EL TIEMPO DE LA PRIMER INTERRUPCION Y EL QUE LE SIGUE, PARA MOSTRAR EN PUERTO SERIE CUANTO TARDA

      TIEMPO3 = HORAS_LCD;
      TIEMPO2 = MINUTOS_LCD;
      TIEMPO = SEGUNDOS_LCD; // MAPEA LOS SEGUNDOS EN LA VARIABLE TIEMPO PARA REPLAZAR Y PODER HACER EL CALCULO DE DIFERENCIA EN LA PROXIMA INTERRUPCIÓN

      if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
      {
        Serial.println("LADO: A"); // Por puerto serie imprime el lado tambien en el que se encuentra
      }
      if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
      {
        Serial.println("LADO: B"); // Por puerto serie imprime el lado tambien en el que se encuentra
      }
      //**************************************** IMPRESION DE VALORES EN LCD ****************************************//

      myFile = SD.open("PRUEBA.txt", FILE_WRITE);
      if (myFile)
      {
        Serial.print("AGUA: "); // imprime por puerto serie el agua medida
        Serial.println(CONT_LCD); // valor del agua

        Serial.print("INTERRUPCIÓN: "); // imprime por puerto serie el tiempo entre medidas
        Serial.print(DIAS_LCD);
        Serial.print(":");
        Serial.print(DIFERENCIA3); // valor del tiempo entre medidas
        Serial.print(":");
        Serial.print(DIFERENCIA2);
        Serial.print(":");
        Serial.println(DIFERENCIA);

        Serial.print("TIEMPO GENERAL: "); // imprime por puerto serie el tiempo entre medidas
        Serial.print(DIAS);
        Serial.print(":");
        Serial.print(HORAS); // valor del tiempo entre medidas
        Serial.print(":");
        Serial.print(MINUTOS);
        Serial.print(":");
        Serial.println(SEGUNDOS);
        Serial.println();

        digitalWrite(13, 1);

        myFile.print("AGUA: "); // imprime por puerto serie el agua medida
        myFile.println(CONT_LCD); // valor del agua

        myFile.print("INTERRUPCIÓN: "); // imprime por puerto serie el tiempo entre medidas
        myFile.print(DIAS_LCD);
        myFile.print(":");
        myFile.print(DIFERENCIA3); // valor del tiempo entre medidas
        myFile.print(":");
        myFile.print(DIFERENCIA2);
        myFile.print(":");
        myFile.println(DIFERENCIA);

        myFile.print("TIEMPO GENERAL: "); // imprime por puerto serie el tiempo entre medidas
        myFile.print(DIAS);
        myFile.print(":");
        myFile.print(HORAS); // valor del tiempo entre medidas
        myFile.print(":");
        myFile.print(MINUTOS);
        myFile.print(":");
        myFile.println(SEGUNDOS);
        myFile.println();
        myFile.close();

        digitalWrite(13, 0);
      }
      //**************************************** IMPRESION DE VALORES EN LCD ****************************************//
      FLAG = 0; // RESETEO DEL FLAG DEL SERVICIO DE INTERRUPCION
    }
  }
}
//*********************** FLAG ( INTERRUPCIÓN POR FLANCO ) **************************//


//************************** SERVICIO DE INTERRUPCION  *********************//
// CON ANTIREBOTE //
// CADA VES QUE SE INTERRUPME PONE LA FLAG EN 1 //

void AUMENTAR()
{
  if ((millis() - INICIO_TIMER) > LIMITE) //ANTIRREBOTE
  {
    INICIO_TIMER = millis();
    FLAG = 1;
  }
}
//************************************************************************//

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
        Serial.print("MEDICION INICIADA");
        Serial.println();

        Serial.print("PRESIONE * PARA REINICIAR");
        Serial.println();

        Serial.print("PRESIONE - PARA CONTINUAR");
        Serial.println();
        Serial.println();

        digitalWrite(13, 1);

        myFile.print("MEDICION INICIADA");
        myFile.println();

        myFile.print("PRESIONE * PARA REINICIAR");
        myFile.println();

        myFile.print("PRESIONE - PARA CONTINUAR");
        myFile.println();
        myFile.println();
        myFile.close();

        digitalWrite(13, 0);
      }
      FLAG4 = 1;
    }

    if (ESTADO == '-') // cuando presione - pausará la medición
    {
      myFile = SD.open("PRUEBA.txt", FILE_WRITE);
      if (myFile)
      {
        digitalWrite(13, 1);
        Serial.print("MEDICION PAUSADA");
        Serial.println();

        Serial.print("PRESIONE + PARA CONTINUAR");
        Serial.println();

        Serial.print("PRESIONE * PARA REINICIAR");
        Serial.println();

        myFile.print("MEDICION PAUSADA");
        myFile.println();

        myFile.print("PRESIONE + PARA CONTINUAR");
        myFile.println();

        myFile.print("PRESIONE * PARA REINICIAR");
        myFile.println();
        myFile.close();
        digitalWrite(13, 0);
      }
      FLAG4 = 0;
    }

    if (ESTADO == '*')
    {
      myFile = SD.open("PRUEBA.txt", FILE_WRITE);
      if (myFile)
      {
        Serial.print("MEDICION FINALIZADA... REINICIANDO");
        myFile.print("MEDICION FINALIZADA... REINICIANDO");
        myFile.close();
      }

      CONTADOR = 0;
      CONT = 0;
      CONT_LCD = 0;

      SEGUNDOS_LCD = 0;
      MINUTOS_LCD = 0;
      HORAS_LCD = 0;
      DIAS_LCD = 0;

      TIEMPO = 0;
      TIEMPO2 = 0;
      TIEMPO3 = 0;

      DIFERENCIA = 0;
      DIFERENCIA2 = 0;
      DIFERENCIA3 = 0;

      FLAG = 0;
      FLAG2 = 0;
      FLAG4 = 0;
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
    SEGUNDOS_LCD ++;
    FLAG2 = 1;
  }
}
//************************************************************************//
