//********************** Liberias **************************************//
#include <LiquidCrystal.h>
#include <EEPROM.h>
//********************** Liberias **************************************//


//********************** definicion de pines LCD ***********************//
#define LEDLCD 8
const int rs = 10, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//********************** definicion de pines LCD ***********************//


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

int TIEMPO = 0;
int DIFERENCIA = 0;

int PAR = 0;

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;
//********************* definicion de variables ************************//


//********* ATENCION EEPROM ************//
//********float PROMEDIO = 5.5; ******//
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

  TCNT1 = 0xC2F8;

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
  
//******************************* SERVICIO DE INTERRUPCIÓN *********************************//


//******************************* inicializacion de variables ******************************//
  bool FLAG = false;
  bool FLAG2 = false;
  bool FLAG3 = true;
  
  CTE = 50 / EEPROMEDIO;
//******************************* inicializacion de variables ******************************//


//************************ INICIALIZACION DE PINES DEL LCD ***********************//

  pinMode(LEDLCD, OUTPUT);
  digitalWrite (LEDLCD, HIGH);
  
//************************ INICIALIZACION DE PANTALLA LCD ***********************//
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("AGUA: 0.00");
  lcd.setCursor(0, 1);
  lcd.print("00:00:00");
  lcd.setCursor(15, 1);
  lcd.print("B");
  lcd.setCursor(10,1);
  lcd.print("0");
//************************ INICIALIZACION DE PANTALLA LCD ***********************//
}

//**********************************************************************************************//


//********************************************** VOID LOOP ****************************************//
void loop()
{
//************************ SETEO DE MINUTOS ************************************//
  if (SEGUNDOS_LCD == 60) // SI ALCANZA LOS 60 SEGUNDOS AUMENTA EN 1 EL MINUTO
  {
    MINUTOS_LCD ++; //aumento en 1 la cantidad de minutos
    SEGUNDOS_LCD = 0; // resetea la variable segundos
    lcd.leftToRight(); // setea el cursor para que escriba de izquierda a derecha
    lcd.setCursor(6, 1); // setea el cursor en la posicion 6 en la linea 1
    lcd.print("00"); // reseteo en display segundos
  }
//************************ SETEO DE MINUTOS ************************************//


//************************* SETEO DE HORAS *************************************//
  if (MINUTOS_LCD == 60) // SI ALCANZA LOS 60 MINUTOS AUMENTA EN 1 LA HORA
  {
    HORAS_LCD ++; //aumento en 1 la cantidad de horas
    MINUTOS_LCD = 0; // reseteo la variable minutos
    lcd.leftToRight(); // setea el cursor para que sea de izquierda a derecha
    lcd.setCursor(3, 1); // setea el cursor en la posicion 3 linea 1
    lcd.print("00"); // reseteo en display minutos
  }
//************************* SETEO DE HORAS *************************************//


//************************ RESETEO DE HORAS ************************************//
  if (HORAS_LCD > 23) // SI ALCANZA UN VALOR MAYOR A 24 RESETEA LA VARIABLE HORAS Y RESETEA EL DISPLAY
  {
    HORAS_LCD = 0; // reseteo la variable en horas
    lcd.leftToRight(); // seteo el cursor para que sea de izquierda a derecha
    lcd.setCursor(0, 1); // setea el cursor en la posición 0 linea 1
    lcd.print("00"); // reseteo en display horas
  }
//************************ RESETEO DE HORAS ************************************//


//*********************** FLAG 2( INTERRUPCIÓN DEL TIMER ) **************************//
  if (FLAG2 == 1)
  {
    lcd.setCursor(2, 1);
    lcd.print(":");
    lcd.setCursor(5, 1);
    lcd.print(":");

    if (HORAS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras las horas son menores a 10, cosa que las horas aparezcan como 01 y no como 1
    {
      lcd.rightToLeft(); // seteo del cursor
      lcd.setCursor(1, 1); // posicion del cursor
      lcd.print(HORAS_LCD); // imprime las horas entre 0 y 9
    }
    if (HORAS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras las horas sean mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21
    {
      lcd.leftToRight(); // seteo del cursor
      lcd.setCursor(0, 1); // posicion del cursor
      lcd.print(HORAS_LCD); // imprimer las horas entre 10 y 23
    }

    if (MINUTOS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras los minutos son menores a 10, cosa que los minutos aparezcan como 01 y no como 1
    {
      lcd.rightToLeft(); //seteo del cursor
      lcd.setCursor(4, 1); // posición del cursor
      lcd.print(MINUTOS_LCD); // imprime los minutos entre 0 y 9
    }
    if (MINUTOS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras los minutos son mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21 siguiente el curso de la linea 176
    {
      lcd.leftToRight(); // seteo del cursor
      lcd.setCursor(3, 1); // posicion del cursor
      lcd.print(MINUTOS_LCD); // imprime los minutos entre 10 y 59
    }
    
    if (SEGUNDOS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras los segundos son menores a 10, cosa que los segundos aparezcan como 01 y no como 1
    {
      lcd.rightToLeft(); // seteo del cursor
      lcd.setCursor(7, 1); // posicion del cursor
      lcd.print(SEGUNDOS_LCD); // imprime los segundos entre 0 y 9
    }
    if (SEGUNDOS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras los segundos son mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21 siguiente el curso de la linea 176
    {
      lcd.leftToRight(); //seteo del cursor
      lcd.setCursor(6, 1); //posición del cursor
      lcd.print(SEGUNDOS_LCD); //imprime los segundos entre 0 y 9
    }
    
    FLAG2 = 0; // reseteo del FLAG2
    
  }
//*********************** FLAG 2( INTERRUPCIÓN DEL TIMER ) **************************//


//*********************** FLAG ( INTERRUPCIÓN POR FLANCO ) **************************//
  if (FLAG == 1) // FLAG del servicio de interrupción 
  {
    FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción
    CONTADOR ++; // AUMENTA EN 1 EL CONTADOR GENERAL, DE ESTE SE BASAN TODOS LOS DEMAS
    CONT = CONTADOR - 1; // ES EL CONTADOR DE CANGILOS QUE SE MUESTRA EN EL LCD
    CONT_LCD = CONTADOR - 1; // ES EL CONTADOR QUE SE USA PARA CALCULAR EL AGUA QUE CALLÓ
    CONT_LCD = CONT_LCD * 9.1; // SE MULTIPLICA POR LA VARIABLE DE MEDIDA DE 1 CANGILON 9.1CM3
    DIFERENCIA = SEGUNDOS_LCD - TIEMPO; // ESTE ES EL CALCULO ENTRE EL TIEMPO DE LA PRIMER INTERRUPCION Y EL QUE LE SIGUE, PARA MOSTRAR EN PUERTO SERIE CUANTO TARDA
    TIEMPO = SEGUNDOS_LCD; // MAPEA LOS SEGUNDOS EN LA VARIABLE TIEMPO PARA REPLAZAR Y PODER HACER EL CALCULO DE DIFERENCIA EN LA PROXIMA INTERRUPCIÓN
    
    if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
    {
      lcd.setCursor(15, 1); // posicion del cursor
      lcd.print(" "); // borro lo que estaba en la posicion de B
      lcd.setCursor(15, 0); // posicion del cursor
      lcd.print("A"); // Imprime en la posicion el lado A (es en el que está)
      Serial.println("LADO: A"); // Por puerto serie imprime el lado tambien en el que se encuentra
    }
    if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
    {
      lcd.setCursor(15, 0); // posicion del cursor
      lcd.print(" "); // borro lo que estaba en la posicion de B
      lcd.setCursor(15, 1); // posicion del cursor
      lcd.print("B"); // Imprime en la posicion el lado B (es en el que está)
      Serial.println("LADO: B"); // Por puerto serie imprime el lado tambien en el que se encuentra
    }
//**************************************** IMPRESION DE VALORES EN LCD ****************************************//
    lcd.leftToRight(); // seteo del cursor
    lcd.setCursor(6, 0); // posicion del cursor
    lcd.print(CONT_LCD); // imprime el agua medida
    lcd.setCursor(10,1); // posicion del cursor
    lcd.print(CONT); // imprime cantidad de cangilones
    
    Serial.print("AGUA: "); // imprime por puerto serie el agua medida
    Serial.println(CONT_LCD); // valor del agua
    
    Serial.print("TIEMPO: "); // imprime por puerto serie el tiempo entre medidas
    Serial.println(DIFERENCIA); // valor del tiempo entre medidas
    Serial.println(" "); // imprime un enter para separar los valores
    
//**************************************** IMPRESION DE VALORES EN LCD ****************************************//
    FLAG = 0; // RESETEO DEL FLAG DEL SERVICIO DE INTERRUPCION
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


//********************************* TIMER 1 *******************************//
// POR DESBORDAMIENTO //
// TIMER MADRE DE 1s  //
// EL FLAG2 SE SETEA EN UNO CADA VEZ QUE INTERRUMPE EL TIMER //
// LA VARIABLE SEGUNDOS_LCD AUMENTA EN 1 CADA VES QUE INTERRUMPE EL TIMER //
ISR(TIMER1_OVF_vect)
{
  TCNT1 = 0xC2F7;

  if (CONTADOR > 0)
  {
    SEGUNDOS_LCD ++;
    FLAG2 = 1;
  }
}
//************************************************************************//
