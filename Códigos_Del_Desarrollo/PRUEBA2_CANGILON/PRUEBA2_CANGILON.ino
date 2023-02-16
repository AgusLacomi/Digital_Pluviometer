#include <LiquidCrystal.h>

#define LEDLCD 14
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile int SET_RECARGA = 0;

unsigned long LIMITE = 10;
unsigned long INICIO_TIMER = 0;

int CONTADOR = 0;

void setup()
{
  SET_RECARGA = 0;

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1<<CS10)|(1<<CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8;

  TIMSK1 |= (1<<TOIE1);//timer open interrupt enable
  
  Serial.begin(9600);
  
  pinMode(LEDLCD, OUTPUT);

  pinMode(18, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(18), AUMENTAR, LOW);

  digitalWrite (LEDLCD, HIGH);
  
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("CONTADOR: ");
  lcd.setCursor(10, 0);    
  lcd.print("0");
}

void loop()
{
}

void AUMENTAR()
{
  if ((millis() - INICIO_TIMER) > LIMITE)
  {
    INICIO_TIMER = millis();
    SET_RECARGA ++;
    Serial.print("CONTADOR: ");
    Serial.println(SET_RECARGA);
    if (SET_RECARGA > 0)
    {
      lcd.setCursor(10, 0);    
      lcd.print(SET_RECARGA);
      Serial.print("TIEMPO: ");
      Serial.println(CONTADOR);
    }    
  }
}

ISR(TIMER1_OVF_vect)
{
  TCNT1=0xC2F7;
  
  if (SET_RECARGA > 0)
  {
    CONTADOR ++;
  }
}
