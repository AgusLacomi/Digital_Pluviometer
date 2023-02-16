//********************** Liberias **************************************//
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
//********************** Liberias **************************************//

File myFile; // Crea una variable myFile de tipo File, para almacenar en la SD la información

//********************* definicion de variables ************************//

volatile float CONTADOR = 0; // Contador en el que se basa el calculo de agua. Si aumenta en 1 se multiplica por 7,9
volatile float AGUA = 0; // variable en la que se almacena el calculo de agua. CONTADOR * 7,9
float CTE; // 7,9 Medicion minima del cangilon

long SEGUNDOS_GRAL = 0; // VARIABLE PRINCIPAL en esta se basan todos los tiempos, es la que aumenta en 1 en el servicio de interrupción del reloj

unsigned long LIMITE = 10;
unsigned long INICIO_TIMER = 0;

long  SEGUNDOS_ENCENDIDO = 0; // Variable en la que se basan las de debajo transformando segundos a minutos horas y dias
int  MINUTOS_ENCENDIDO = 0;
int  HORAS_ENCENDIDO = 0;
int DIAS_ENCENDIDO = 0;

long SEGUNDOS_INT = 0; // Varialble de tiempo ENTRE interrupciones del cangilón, mismo sistema que la variable anterior en la que las demas se basan en esta
int MINUTOS_INT = 0;
int HORAS_INT = 0;
int DIAS_INT = 0;

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;
bool ENCENDIDO = 0;
bool FLAG5 = 0;

bool puertoSD = false; // Inicialización de SD correcta
//********************* definicion de variables ************************//


//********* ATENCION EEPROM ************//

float EEPROMEDIO; // NO SE USA PERO HAY QUE TENERLA EN CUENTA YA QUE FUE CONFIGURADA ANTERIORMENTE

//********* ATENCION EEPROM ************//


//***************************************** SETUP *********************************************//

void setup()
{
  // TIMER 1 //
  // TIMER POR DESBORDAMIENTO CON PRESCALER 1024 //
  // TIENE UN PERIODO DE 1S FRECUENCIA DE 1 HZ MARCA EL INICIO DE LA SEÑAL //

  //delay(1000);

  CONTADOR = 0; // inicialización
  AGUA = 0; // inicialización

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8; //  F9E5 100 MS

  TIMSK1 |= (1 << TOIE1); //timer open interrupt enable
  //********************************* TIMER 1 ********************************//


  //****************************** Inicializacion puerto serie *******************************//
  Serial.begin(9600);
  //****************************** Inicializacion puerto serie *******************************//


  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW); // Configuración del servicio de interrupción, Interrupción del cangilón

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), CONTROL, LOW); // Configuración del servicio de interrupción, Control de encendido y apagado

  //******************************* SERVICIO DE INTERRUPCIÓN *********************************//


  //******************************* inicializacion de variables ******************************//

  FLAG = false;
  FLAG2 = false;
  FLAG3 = true;
  ENCENDIDO = false;
  FLAG5 = false;

  CTE = 7.90;

  //******************************* inicializacion de variables ******************************//

  Serial.print("INICIANDO SD ...");

  puertoSD = SD.begin(53) ; // Inicialización puerto SD

  if (!puertoSD) { // Si no se inicializa el puertoSD entra en bucle y hay que reiniciar el arduino
    Serial.println("ALGO FALLO!");
    while (1);
  }
  Serial.println("TODO JOYA"); // Si inicia todo correctamente continua

  //SD.remove("ENSAYO.txt");

  pinMode(13, OUTPUT); // setea el pin que te indica si esta funcionando el pluviometro

  //************************ INICIALIZACION DE PINES DEL LCD ***********************//

  digitalWrite(13, 1);
  Serial.println("PRESIONE PULSADOR PARA INICIAR O PAUSAR LA MEDICION");
  Serial.println();
  digitalWrite(13, 0); // SI EN EL ARDUINO SE VE EL LED APAGADO ES PORQUE ESTA FUNCIONANDO CORRECTAMENTE Y HAY QUE INICIAR LA MEDICION CON EL PULSADO
}

//**********************************************************************************************//


//********************************************** VOID LOOP ****************************************//
void loop()
{
  if (ENCENDIDO && FLAG) // ENCENDIDO = Flag perteneciente al encendido del arduino del servicio de interrupción y FLAG = Interrupción del cangilón
  {
    myFile = SD.open("ENSAYO.txt", FILE_WRITE); // Configuración del log del SD

    FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción para cambiar el lado del cangilón 
    CONTADOR++; // AUMENTA EN 1 EL CONTADOR GENERAL, DE ESTE SE BASAN TODOS LOS DEMAS
    AGUA = CONTADOR * 7.9; // SE MULTIPLICA POR LA VARIABLE DE MEDIDA DE 1 CANGILON 7.9CM3

    SEGUNDOS_ENCENDIDO = SEGUNDOS_GRAL ;

    if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
    {
      Serial.print("A;"); // Por puerto serie imprime el lado tambien en el que se encuentra
    }
    if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
    {
      Serial.print("B;"); // Por puerto serie imprime el lado tambien en el que se encuentra
    }

    Serial.print(AGUA); // valor del agua
    Serial.print(";");

    //TIEMPO GENERAL QUE SE MUESTRA EN SOFTWARE
    DIAS_ENCENDIDO = SEGUNDOS_ENCENDIDO / 86400 ;
    SEGUNDOS_ENCENDIDO = SEGUNDOS_ENCENDIDO - DIAS_ENCENDIDO * 86400 ;
    HORAS_ENCENDIDO = SEGUNDOS_ENCENDIDO / 3600 ;
    SEGUNDOS_ENCENDIDO = SEGUNDOS_ENCENDIDO - HORAS_ENCENDIDO * 3600 ;
    MINUTOS_ENCENDIDO = SEGUNDOS_ENCENDIDO / 60 ;
    SEGUNDOS_ENCENDIDO = SEGUNDOS_ENCENDIDO - MINUTOS_ENCENDIDO * 60;

    Serial.print (DIAS_ENCENDIDO);
    Serial.print (":");
    Serial.print (HORAS_ENCENDIDO);
    Serial.print (":");
    Serial.print (MINUTOS_ENCENDIDO);
    Serial.print (":");
    Serial.print (SEGUNDOS_ENCENDIDO);
    Serial.print (";");

    // TIEMPO INTERRUPCIÓN y su calculo
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
      Serial.println (";");
    }

    if (myFile) { //grabado en la SD

      digitalWrite(13, 1);

      if (FLAG3)
      {

      } else if (!FLAG3)
      {
        myFile.print("B;");
      }

      //AGUA
      myFile.print(AGUA);
      myFile.print(";");

      //TIEMPO GENERAL QUE SE MUESTRA EN SOFTWARE
      myFile.print (DIAS_ENCENDIDO);
      myFile.print (":");
      myFile.print (HORAS_ENCENDIDO);
      myFile.print (":");
      myFile.print (MINUTOS_ENCENDIDO);
      myFile.print (":");
      myFile.print (SEGUNDOS_ENCENDIDO);
      myFile.print (";");

      //TIEMPO INTERRUPCION
      if (SEGUNDOS_INT > 0)
      {
        myFile.print (DIAS_INT);
        myFile.print (":");
        myFile.print (HORAS_INT);
        myFile.print (":");
        myFile.print (MINUTOS_INT);
        myFile.print (":");
        myFile.print (SEGUNDOS_INT);
        myFile.println (";");
        myFile.close();
      }

      digitalWrite(13, 0);

    }

    SEGUNDOS_INT = SEGUNDOS_GRAL ;

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
    ENCENDIDO = ! ENCENDIDO;
    if (ENCENDIDO) // si está encendido esta prendido el LED
    {
      Serial.println("Medicion iniciada");
      digitalWrite(13, 1);
    }
    if (!ENCENDIDO) // si esta apagado esta apagado el LED
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
  TCNT1 = 0xC2F7; // F9E4   100 ms

  if (ENCENDIDO)
  {
    SEGUNDOS_GRAL ++;
    FLAG2 = 1;
  }
}
//************************************************************************//
