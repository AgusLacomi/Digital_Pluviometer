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

  //delay(1000);
  
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
  attachInterrupt(digitalPinToInterrupt(3), CONTROL, LOW);

  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//


  //******************************* inicializacion de variables ******************************//
  FLAG = false;
  FLAG2 = false;
  FLAG3 = true;
  FLAG4 = false;
  FLAG5 = false;


  CTE = 7.90;

  //******************************* inicializacion de variables ******************************//

  //SD.remove("ENSAYO.txt");

  pinMode(13, OUTPUT);

  //************************ INICIALIZACION DE PINES DEL LCD ***********************//

  digitalWrite(13, 1);
  Serial.println("PRESIONE PULSADOR PARA INICIAR O PAUSAR LA MEDICION");
  Serial.println();
  digitalWrite(13, 0);
}

//**********************************************************************************************//


//********************************************** VOID LOOP ****************************************//
void loop()
{
  if (FLAG4 == 1)
  {
    if (FLAG == 1) // FLAG del servicio de interrupción
    {
      FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción
      CONTADOR ++; // AUMENTA EN 1 EL CONTADOR GENERAL, DE ESTE SE BASAN TODOS LOS DEMAS
      CONT_LCD = CONTADOR * 7.9; // SE MULTIPLICA POR LA VARIABLE DE MEDIDA DE 1 CANGILON 7.9CM3

      SEGUNDOS_SFTW = SEGUNDOS_GRAL ;

      if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
      {
        Serial.print("A;"); // Por puerto serie imprime el lado tambien en el que se encuentra
      }
      if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
      {
        Serial.print("B;"); // Por puerto serie imprime el lado tambien en el que se encuentra
      }

      Serial.print(CONT_LCD); // valor del agua
      Serial.print(";");
      
      // TIEMPO INTERRUPCIÓN
      if (SEGUNDOS_INT > 0)
      {
        SEGUNDOS_INT = SEGUNDOS_GRAL - SEGUNDOS_INT ;

        DIAS_INT = SEGUNDOS_INT / 86400 ;
        SEGUNDOS_INT = SEGUNDOS_INT - DIAS_INT * 86400 ;
        HORAS_INT = SEGUNDOS_INT / 3600 ;
        SEGUNDOS_INT = SEGUNDOS_INT - HORAS_INT * 3600 ;
        MINUTOS_INT = SEGUNDOS_INT / 60 ;
        SEGUNDOS_INT = SEGUNDOS_INT - MINUTOS_INT * 60;

        Serial.print (DIAS_INT);
        Serial.print (":");
        Serial.print (HORAS_INT);
        Serial.print (":");
        Serial.print (MINUTOS_INT);
        Serial.print (":");
        Serial.print (SEGUNDOS_INT);
      }

      SEGUNDOS_INT = SEGUNDOS_GRAL ;

      //TIEMPO GENERAL QUE SE MUESTRA EN SOFTWARE
      DIAS_SFTW = SEGUNDOS_SFTW / 86400 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - DIAS_SFTW * 86400 ;
      HORAS_SFTW = SEGUNDOS_SFTW / 3600 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - HORAS_SFTW * 3600 ;
      MINUTOS_SFTW = SEGUNDOS_SFTW / 60 ;
      SEGUNDOS_SFTW = SEGUNDOS_SFTW - MINUTOS_SFTW * 60;

      Serial.print (DIAS_SFTW);
      Serial.print (":");
      Serial.print (HORAS_SFTW);
      Serial.print (":");
      Serial.print (MINUTOS_SFTW);
      Serial.print (":");
      Serial.println (SEGUNDOS_SFTW);

      //**************************************** IMPRESION DE VALORES EN LCD ****************************************//
      FLAG = 0; // RESETEO DEL FLAG DEL SERVICIO DE INTERRUPCION
  }
}


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

void CONTROL()
{
  if ((millis() - INICIO_TIMER) > LIMITE)
  {
    INICIO_TIMER = millis();
    FLAG4 = ! FLAG4;
    if(FLAG4)
    {
      Serial.println("Medicion iniciada");
      digitalWrite(13, 1);
    }
    if(!FLAG4)
    {
      Serial.println("Medicion pausada");
      digitalWrite(13, 0);
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
