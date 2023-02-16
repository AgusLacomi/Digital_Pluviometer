#include <LiquidCrystal.h>

#define LED 14
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile int SET_RECARGA = 0;

unsigned long LIMITE = 50;
unsigned long INICIO_TIMER = 0;

void setup()
{
  Serial.begin(9600);
  
  pinMode(LED, OUTPUT);

  pinMode(18, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(18), AUMENTAR, LOW);

  digitalWrite (LED, HIGH);
  
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
    lcd.setCursor(10, 0);    
    lcd.print(SET_RECARGA);
    Serial.print("CONTADOR: ");
    Serial.println(SET_RECARGA);
  }
}
