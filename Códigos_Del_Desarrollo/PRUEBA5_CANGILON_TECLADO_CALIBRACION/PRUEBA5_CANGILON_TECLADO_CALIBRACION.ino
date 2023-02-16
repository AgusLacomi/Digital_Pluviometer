#include <LiquidCrystal.h>
#include <EEPROM.h>

#define LEDLCD 8
const int rs = 10, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

volatile int CONT_MEDICION = 0;
volatile int MEDICION1 = 0;
volatile int MEDICION2 = 0;
volatile int MEDICION3 = 0;
volatile int MEDICION4 = 0;
volatile int MEDICION5 = 0;
volatile int MEDICION6 = 0;
volatile int MEDICION7 = 0;
volatile int MEDICION8 = 0;
volatile int MEDICION9 = 0;
volatile int MEDICION10 = 0;

//*********************
//float PROMEDIO = 5.5;
float EEPROMEDIO;
//*********************

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;
bool FLAG_MEDICION = false;

void setup()
{
  CONTADOR = 0;
  CONT_LCD = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8;

  TIMSK1 |= (1 << TOIE1); //timer open interrupt enable

  //******************** Grabo en EEPROM el valor promedio *******************

  // EEPROM.put (EEPROMEDIO, PROMEDIO);

  //***************************************************************************

  Serial.begin(9600);

  pinMode(LEDLCD, OUTPUT);

  pinMode(2, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW);

  digitalWrite (LEDLCD, HIGH);

  bool FLAG = false;
  bool FLAG2 = false;
  bool FLAG3 = true; 

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("AGUA: 0.00");
  lcd.setCursor(0, 1);
  lcd.print("00:00:00");
  lcd.setCursor(15, 1);
  lcd.print("B");
  lcd.setCursor(10, 1);
  lcd.print("0");
}

void loop()
{
  SERIAL_EVENT();

  if (SEGUNDOS_LCD == 60)
  {
    MINUTOS_LCD ++;
    SEGUNDOS_LCD = 0;
    lcd.leftToRight();
    lcd.setCursor(6, 1);
    lcd.print("00");
  }

  if (MINUTOS_LCD == 60)
  {
    HORAS_LCD ++;
    MINUTOS_LCD = 0;
    lcd.leftToRight();
    lcd.setCursor(3, 1);
    lcd.print("00");
  }

  if (HORAS_LCD > 23)
  {
    HORAS_LCD = 0;
    lcd.leftToRight();
    lcd.setCursor(0, 1);
    lcd.print("00");
  }

  if (FLAG2 == 1)
  {
    if (HORAS_LCD < 10)
    {
      lcd.rightToLeft();
      lcd.setCursor(1, 1);
      lcd.print(HORAS_LCD);
    }
    if (HORAS_LCD >= 10)
    {
      lcd.leftToRight();
      lcd.setCursor(0, 1);
      lcd.print(HORAS_LCD);
    }

    if (MINUTOS_LCD < 10)
    {
      lcd.rightToLeft();
      lcd.setCursor(4, 1);
      lcd.print(MINUTOS_LCD);
    }
    if (MINUTOS_LCD >= 10)
    {
      lcd.leftToRight();
      lcd.setCursor(3, 1);
      lcd.print(MINUTOS_LCD);
    }

    if (SEGUNDOS_LCD < 10)
    {
      lcd.rightToLeft();
      lcd.setCursor(7, 1);
      lcd.print(SEGUNDOS_LCD);
    }
    if (SEGUNDOS_LCD >= 10)
    {
      lcd.leftToRight();
      lcd.setCursor(6, 1);
      lcd.print(SEGUNDOS_LCD);
    }
    FLAG2 = 0;
  }

  if (FLAG_MEDICION)
  {
    CONTADOR = 0;
    TIEMPO = 0;
    SEGUNDOS_LCD = 0;
    Serial.println (" ");
    Serial.println("VARIABLES REINICIADAS");
    FLAG_MEDICION = 0;
  }

  if (FLAG == 1)
  {
    FLAG3 = ! FLAG3;
    CONTADOR ++;

    CONT = CONTADOR - 1; //MUESTRA EN LCD
    CONT_LCD = CONTADOR - 1;
    DIFERENCIA = SEGUNDOS_LCD - TIEMPO;
    TIEMPO = SEGUNDOS_LCD;
    
    if (FLAG3)
    {
      lcd.setCursor(15, 1);
      lcd.print(" ");
      lcd.setCursor(15, 0);
      lcd.print("A");
      Serial.println("LADO: A");
    }
    if (!FLAG3)
    {
      lcd.setCursor(15, 0);
      lcd.print(" ");
      lcd.setCursor(15, 1);
      lcd.print("B");
      Serial.println("LADO: B");
    }
    lcd.leftToRight();
    lcd.setCursor(6, 0);
    lcd.print(CONT_LCD);
    lcd.setCursor(10, 1);
    lcd.print(CONT);
    Serial.print("AGUA: ");
    Serial.println(CONT_LCD);
    Serial.print("TIEMPO: ");
    Serial.println(DIFERENCIA);
    Serial.println(" ");
    FLAG = 0;
  }
}

void SERIAL_EVENT()
{
  while (Serial.available())
  {
    int ESTADO = '0';

    ESTADO = Serial.read();

    if (ESTADO == '+')
    {
      CONT_MEDICION ++;
      FLAG_MEDICION = 1;

      Serial.println(" ");
      Serial.println("CAMBIO DE MEDICION EFECTUADO");
      Serial.print("ACTUALMENTE EN MEDICIÓN: ");
      Serial.print(CONT_MEDICION);
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
