//********************** Liberias **************************************//
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

long SEGUNDOS_GRAL = 0;

long  SEGUNDOS_SFTW = 0;
int  MINUTOS_SFTW = 0;
int  HORAS_SFTW = 0;
int DIAS_SFTW = 0;

long SEGUNDOS_INT = 0;
int MINUTOS_INT = 0;
int HORAS_INT = 0;
int DIAS_INT = 0;

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;
bool FLAG4 = 0;
bool FLAG5 = 0;
//********************* definicion de variables ************************//


//********* ATENCION EEPROM ************//

float EEPROMEDIO;

//********* ATENCION EEPROM ************//


//***************************************** SETUP *********************************************//

void setup()
{
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
  Serial.begin(9600);
  //****************************** Inicializacion puerto serie *******************************//


  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW);

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), INICIAR, HIGH);

  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//


  //******************************* inicializacion de variables ******************************//
  FLAG = false;
  FLAG2 = false;
  FLAG3 = true;
  FLAG4 = false;


  CTE = 7.90;

  //******************************* inicializacion de variables ******************************//

  Serial.print("INICIANDO SD ...");

  FLAG5 = SD.begin(53);
  
  if (!FLAG5) {
    Serial.println("ALGO FALLO!");
    while (1);
  }
  Serial.println("TODO JOYA");

  //SD.remove("ENSAYO.txt");

  pinMode(13, OUTPUT);

  //************************ INICIALIZACION DE PINES DEL LCD ***********************//

  myFile = SD.open("ENSAYO.txt", FILE_WRITE);
  if (myFile)
  {
    Serial.println("PRESIONE + PARA INICIAR");
    Serial.println("PRESIONE - PARA PAUSAR");
    Serial.println("PRESIONE * PARA REINICIAR");
    Serial.println();

    digitalWrite(13, 1);
    myFile.println("PRESIONE + PARA INICIAR");
    myFile.println("PRESIONE - PARA PAUSAR");
    myFile.println("PRESIONE * PARA REINICIAR");
    myFile.println();
    myFile.close();
    digitalWrite(13, 0);
  }
  else
  {
    Serial.println("ERROR ABRIENDO ENSAYO.txt");
  }
}

//**********************************************************************************************//


//********************************************** VOID LOOP ****************************************//
void loop()
{
  SERIAL_EVENT();
  if (FLAG4 == 1)
  {
    if (FLAG == 1) // FLAG del servicio de interrupción
    {
      FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción
      CONTADOR ++; // AUMENTA EN 1 EL CONTADOR GENERAL, DE ESTE SE BASAN TODOS LOS DEMAS
      CONT_LCD = CONTADOR * 7.9; // SE MULTIPLICA POR LA VARIABLE DE MEDIDA DE 1 CANGILON 7.9CM3

      SEGUNDOS_SFTW = SEGUNDOS_GRAL ;

      myFile = SD.open("ENSAYO.txt", FILE_WRITE);
      if (myFile)
      {
        if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
        {
          myFile.print("A;");
          Serial.print("A;"); // Por puerto serie imprime el lado tambien en el que se encuentra
        }


        if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
        {
          myFile.print("B;"); // Por puerto serie imprime el lado tambien en el que se encuentra
          Serial.print("B;"); // Por puerto serie imprime el lado tambien en el que se encuentra
        }
      }

      if (myFile)
      {
        digitalWrite(13, 1);
        myFile.print(CONT_LCD);
        myFile.print(";");
        digitalWrite(13, 0);

        Serial.print(CONT_LCD); // valor del agua
        Serial.print(";");
      }

// TIEMPO INTERRUPCIÓN
      if (SEGUNDOS_INT > 0)
      {
        SEGUNDOS_INT = SEGUNDOS_SFTW - SEGUNDOS_INT ;

        DIAS_INT = SEGUNDOS_INT / 86400 ;
        SEGUNDOS_INT = SEGUNDOS_INT - DIAS_INT * 86400 ;
        HORAS_INT = SEGUNDOS_INT / 3600 ;
        SEGUNDOS_INT = SEGUNDOS_INT - HORAS_INT * 3600 ;
        MINUTOS_INT = SEGUNDOS_INT / 60 ;
        SEGUNDOS_INT = SEGUNDOS_INT - MINUTOS_INT * 60;

        if (myFile)
        {
          digitalWrite(13, 1);
          myFile.print (DIAS_INT);
          myFile.print (":");
          myFile.print (HORAS_INT);
          myFile.print (":");
          myFile.print (MINUTOS_INT);
          myFile.print (":");
          myFile.print (SEGUNDOS_INT);
          myFile.print (";");
          digitalWrite(13, 0);

          Serial.print (DIAS_INT);
          Serial.print (":");
          Serial.print (HORAS_INT);
          Serial.print (":");
          Serial.print (MINUTOS_INT);
          Serial.print (":");
          Serial.print (SEGUNDOS_INT);
          Serial.print (";");
        }
      }

      SEGUNDOS_INT = SEGUNDOS_GRAL ;

      //TIEMPO GENERAL QUE SE MUESTRA EN SOFTWARE
      DIAS_SFTW = SEGUNDOS_SFTW / 86400 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - DIAS_SFTW * 86400 ;
      HORAS_SFTW = SEGUNDOS_SFTW / 3600 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - HORAS_SFTW * 3600 ;
      MINUTOS_SFTW = SEGUNDOS_SFTW / 60 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - MINUTOS_SFTW * 60;

      if (myFile)
      {
        digitalWrite(13, 1);
        myFile.print (DIAS_SFTW);
        myFile.print (":");
        myFile.print (HORAS_SFTW);
        myFile.print (":");
        myFile.print (MINUTOS_SFTW);
        myFile.print (":");
        myFile.println (SEGUNDOS_SFTW);
        myFile.close();
        digitalWrite(13, 0);

        Serial.print (DIAS_SFTW);
        Serial.print (":");
        Serial.print (HORAS_SFTW);
        Serial.print (":");
        Serial.print (MINUTOS_SFTW);
        Serial.print (":");
        Serial.println (SEGUNDOS_SFTW);
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

void INICIAR()
{
  if ((millis() - INICIO_TIMER) > LIMITE)
  {
    INICIO_TIMER = millis();
    FLAG4 = 1;
  }
}

void SERIAL_EVENT()
{
  while (Serial.available()) // lectura del teclado
  {
    int ESTADO = '0'; // 0 ASCII

    ESTADO = Serial.read(); // La variable estado pasara a valer lo que detecte del teclado

    if (ESTADO == '/') // Mensaje para el software
    {
      Serial.println("Comunicacion iniciada");
    }

    if (ESTADO == '-') // cuando presione - pausará la medición
    {
      FLAG4 = 0;
      
      Serial.print("Medicion pausada");
    }

    if (ESTADO == '*') // Reinicio de programa
    {
      Serial.print("Medicion reiniciada");
      
      CONTADOR = 0;
      CONT = 0;
      CONT_LCD = 0;

      SEGUNDOS_GRAL = 0;

      SEGUNDOS_SFTW = 0;
      MINUTOS_SFTW = 0;
      HORAS_SFTW = 0;
      DIAS_SFTW = 0;

      SEGUNDOS_INT = 0;
      MINUTOS_INT = 0;
      HORAS_INT = 0;
      DIAS_INT = 0;

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
  TCNT1 = 0xC2F7; // F9E4 100 ms

  if (FLAG4)
  {
    SEGUNDOS_GRAL ++;
    FLAG2 = 1;
  }
}
//************************************************************************//
