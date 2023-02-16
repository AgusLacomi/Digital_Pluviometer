//********************** Liberias **************************************//
#include <LiquidCrystal.h>
#include <EEPROM.h>
//********************** Liberias **************************************//

//********************** definicion de pines LCD ***********************//
#define LEDLCD 8
const int rs = 10, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//********************** definicion de pines LCD ***********************//

typedef struct {
  int PIVOTEOS;
  int TIEMPO [30];
  float TIEMPO_PROM;
} MUESTRA;

MUESTRA PLANILLA[10];

int CANT_AGUA = 200;
int CONTADOR_ARRAY;

int CONTADOR_PANTALLA = 1;
volatile int CONTADOR = 0;
volatile int CONT = 0;
volatile int CONT_LCD = 0;
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

  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW);

  bool FLAG = false;
  bool FLAG2 = false;
  bool FLAG3 = true;

  pinMode(LEDLCD, OUTPUT);
  digitalWrite (LEDLCD, HIGH);
  
  for(int i=0; i<10 ; i ++)
  {
    CANGILONES [i] = 0;
  }
  
  AGUA = 0;
  PROMEDIO = 0;
  CONTADOR = 0;
  CONTADOR_ARRAY = 0;
  CONTADOR_PANTALLA = 1;

  Serial.print ("COMIENZO DE LA PRUEBA: ");
  Serial.print (CONTADOR_PANTALLA);
  Serial.println ();
  Serial.println ();
  Serial.println ();
}

void loop()
{
  SERIAL_EVENT();
  
   if (FLAG == 1)
  {
    CONTADOR ++;
    CONT = CONTADOR - 1;
    CONT_LCD = CONTADOR - 1;
    CANGILONES [CONTADOR_ARRAY] = CONT_LCD;
    
    DIFERENCIA = SEGUNDOS_LCD - TIEMPO;
    TIEMPO = SEGUNDOS_LCD;
    
    Serial.print("CANGILONES: ");
    Serial.println(CANGILONES [CONTADOR_ARRAY]);
    Serial.print("TIEMPO: ");
    Serial.println(DIFERENCIA);
    Serial.println();
    
    FLAG = 0;
  }
  
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

  if (CONTADOR_ARRAY == 9)
  {
    for (int i=0; i<10; i++)
    {
      PROMEDIO = PROMEDIO + CANGILONES[i]; 
    }
    
    PROMEDIO = PROMEDIO / CONTADOR_PANTALLA;
    
    Serial.println();
    Serial.print ("EL PROMEDIO LOGRADO ES: ");
    Serial.print (PROMEDIO);
    Serial.println();
    Serial.println();
  }
  
}
  


///////////////////////////////////////////////////////////////////////////////////////

void SERIAL_EVENT()
{
  while (Serial.available())                  // HARA LO SIGUIENTE SI DETECTA QUE SE HA HABILITADO EL PUERTO SERIE
  {
    int ESTADO = '0';                         // DECLARACION DE VARIABLE QUE ALMACENA LA TECLA PRESIONADA

    ESTADO = Serial.read();                   // ALMACENA EN LA VARIABLE ESTADO LO QUE DETECTA EL TECLADO

    if (ESTADO == '+')                        // SI SE PRESIONA LA TECLA + AUMENTARA EN 5 SET_RECARGA Y MOSTRARA POR PUERTO SERIE QUE POSICION ESTÁ
    {                                         // SI SE EXCEDE DE 130 VOLVERÁ A SU POSICION INICIAL Y MOSTRARA "VALOR EXCEDIDO"
      CONTADOR_ARRAY ++;
      CONTADOR_PANTALLA ++;
      
      CONTADOR = 0;
      CONT = 0;
      CONT_LCD = 0;
      DIFERENCIA = 0;
      TIEMPO = 0;
      
      SEGUNDOS_LCD = 0;
      MINUTOS_LCD = 0;
      HORAS_LCD = 0;


      Serial.println ();
      Serial.print ("COMIENZO DE LA PRUEBA: ");
      Serial.print (CONTADOR_ARRAY);
      Serial.println ();
      Serial.println ();
      Serial.println ();
    }
  }
}

void AUMENTAR()
{
  if ((millis() - INICIO_TIMER) > LIMITE)
  {
    INICIO_TIMER = millis();
    FLAG = 1;
  }
}

ISR(TIMER1_OVF_vect)
{
  TCNT1 = 0xC2F7;

  if (CONTADOR > 0)
  {
    SEGUNDOS_LCD ++;
    FLAG2 = 1;
  }
}





/////////////////////////////////////////////////////////////////////////////////////////

//  //************************ SETEO DE MINUTOS ************************************//
//  if (SEGUNDOS_LCD == 60) // SI ALCANZA LOS 60 SEGUNDOS AUMENTA EN 1 EL MINUTO
//  {
//    MINUTOS_LCD ++; //aumento en 1 la cantidad de minutos
//    SEGUNDOS_LCD = 0; // resetea la variable segundos
//    lcd.leftToRight(); // setea el cursor para que escriba de izquierda a derecha
//    lcd.setCursor(6, 1); // setea el cursor en la posicion 6 en la linea 1
//    lcd.print("00"); // reseteo en display segundos
//  }
////************************ SETEO DE MINUTOS ************************************//
//
//
////************************* SETEO DE HORAS *************************************//
//  if (MINUTOS_LCD == 60) // SI ALCANZA LOS 60 MINUTOS AUMENTA EN 1 LA HORA
//  {
//    HORAS_LCD ++; //aumento en 1 la cantidad de horas
//    MINUTOS_LCD = 0; // reseteo la variable minutos
//    lcd.leftToRight(); // setea el cursor para que sea de izquierda a derecha
//    lcd.setCursor(3, 1); // setea el cursor en la posicion 3 linea 1
//    lcd.print("00"); // reseteo en display minutos
//  }
////************************* SETEO DE HORAS *************************************//
//
//
////************************ RESETEO DE HORAS ************************************//
//  if (HORAS_LCD > 23) // SI ALCANZA UN VALOR MAYOR A 24 RESETEA LA VARIABLE HORAS Y RESETEA EL DISPLAY
//  {
//    HORAS_LCD = 0; // reseteo la variable en horas
//    lcd.leftToRight(); // seteo el cursor para que sea de izquierda a derecha
//    lcd.setCursor(0, 1); // setea el cursor en la posición 0 linea 1
//    lcd.print("00"); // reseteo en display horas
//  }
////************************ RESETEO DE HORAS ************************************//
//
//
//if (FLAG2 == 1)
//  {
//    lcd.setCursor(2, 1);
//    lcd.print(":");
//    lcd.setCursor(5, 1);
//    lcd.print(":");
//
//    if (HORAS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras las horas son menores a 10, cosa que las horas aparezcan como 01 y no como 1
//    {
//      lcd.rightToLeft(); // seteo del cursor
//      lcd.setCursor(1, 1); // posicion del cursor
//      lcd.print(HORAS_LCD); // imprime las horas entre 0 y 9
//    }
//    if (HORAS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras las horas sean mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21
//    {
//      lcd.leftToRight(); // seteo del cursor
//      lcd.setCursor(0, 1); // posicion del cursor
//      lcd.print(HORAS_LCD); // imprimer las horas entre 10 y 23
//    }
//
//    if (MINUTOS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras los minutos son menores a 10, cosa que los minutos aparezcan como 01 y no como 1
//    {
//      lcd.rightToLeft(); //seteo del cursor
//      lcd.setCursor(4, 1); // posición del cursor
//      lcd.print(MINUTOS_LCD); // imprime los minutos entre 0 y 9
//    }
//    if (MINUTOS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras los minutos son mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21 siguiente el curso de la linea 176
//    {
//      lcd.leftToRight(); // seteo del cursor
//      lcd.setCursor(3, 1); // posicion del cursor
//      lcd.print(MINUTOS_LCD); // imprime los minutos entre 10 y 59
//    }
//    
//    if (SEGUNDOS_LCD < 10) // este if lo que hace es poner el cursor de derecha a izquierda mientras los segundos son menores a 10, cosa que los segundos aparezcan como 01 y no como 1
//    {
//      lcd.rightToLeft(); // seteo del cursor
//      lcd.setCursor(7, 1); // posicion del cursor
//      lcd.print(SEGUNDOS_LCD); // imprime los segundos entre 0 y 9
//    }
//    if (SEGUNDOS_LCD >= 10) // este if lo que hace es poner el cursor de izquierda a derecha mientras los segundos son mayores o iguales a 10, por eso aca se da vuelta de nuevo para que el 10 corresponda y no esté como 01, 11, 21 siguiente el curso de la linea 176
//    {
//      lcd.leftToRight(); //seteo del cursor
//      lcd.setCursor(6, 1); //posición del cursor
//      lcd.print(SEGUNDOS_LCD); //imprime los segundos entre 0 y 9
//    }
//    FLAG2 = 0; // reseteo del FLAG2
//  }
