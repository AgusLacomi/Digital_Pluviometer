#include <LiquidCrystal.h>
#include <EEPROM.h>

//********************** definicion de pines LCD ***********************//
#define LEDLCD 8
const int rs = 10, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//********************** definicion de pines LCD ***********************//

//********************** Definicion de estructura ***********************//
typedef struct ESTRUCTURA
{
  int CANGILONES;
  int TIEMPO[30];
  float TIEMPO_PROMEDIO;
} ESTRUCTURA;

ESTRUCTURA p[10];
//********************** Definicion de estructura ***********************//

//********************** Definicion de variables ***********************//
int cont_PLANILLA;
int cont_tiempo;

float PROM_CANGILONES = 0;
float TIEMPO_PROM_GENERAL = 0;
int AGUA = 100;

bool FLAG = 0;
bool FLAG2 = 0;
bool FLAG3 = 0;

int SEGUNDOS_LCD = 0;
int MINUTOS_LCD = 0;
int HORAS_LCD = 0;

int TIEMPO_ANTERIOR = 0;
int SEGUNDOS_ARRAY = 0;

unsigned long LIMITE = 10;
unsigned long INICIO_TIMER = 0;


//********************** Definicion de variables ***********************//


//********************** SETUP ***********************//
void setup()
{
// TIMER 1 //
// TIMER POR DESBORDAMIENTO CON PRESCALER 1024 //
// TIENE UN PERIODO DE 1S FRECUENCIA DE 1 HZ MARCA EL INICIO DE LA SEÑAL //

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10) | (1 << CS12); //para prescaler de 1024 CS12=1 y CS10=1

  TCNT1 = 0xC2F8;

  TIMSK1 |= (1 << TOIE1); //timer open interrupt enable
//********************************* TIMER 1 ********************************//


//********************** SERVICIO DE INTERRUPCIÓN ***********************//
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), AUMENTAR, LOW);
//********************** SERVICIO DE INTERRUPCIÓN ***********************//


//********************** INICIALIZACION DE TYPEDEF STRUCT ***********************//
  for (int i = 0; i < 10; i++)
  {
    p[i].CANGILONES = 0;
    p[i].TIEMPO_PROMEDIO = 0;
    
    for (int e = 0; e < 30; e++)
    {
      p[i].TIEMPO[e] = 0;
    }
  }

  cont_tiempo = cont_tiempo - 1;
//********************** INICIALIZACION DE TYPEDEF STRUCT ***********************//


//********************** INICIALIZACION PUERTO SERIE ***********************//
  Serial.begin(9600);
//********************** INICIALIZACION PUERTO SERIE ***********************//


//********************** INICIALIZACION DE PINES ***********************//
  pinMode(LEDLCD, OUTPUT);
  digitalWrite (LEDLCD, HIGH);
//********************** INICIALIZACION DE PINES ***********************//


//********************** INICIO DE PANTALLA ***********************//
  Serial.println();
  Serial.print("AGUA EN MEDICIÓN: ");
  Serial.println (AGUA);
  Serial.println ();
  Serial.print("PLANILLA: ");
  Serial.println (cont_PLANILLA);
  
//********************** INICIO DE PANTALLA ***********************//
}
//********************** SETUP ***********************//


//********************** LOOP ***********************//
void loop()
{
  SERIAL_EVENT (); // LECTURA DE TECLADO 
  
  if (FLAG == 1) // FLAG DEL SERVICIO DE INTERRUPCIÓN 
  {
    cont_tiempo ++; //muestra la posición del array en la que se encuentra el tiempo de interrupción

    FLAG3 = ! FLAG3; // cambia de estado el FLAG3 cada interrupción para mostrar la posicion en la q esta si en A o B

    p[cont_PLANILLA].CANGILONES ++; //aumenta en 1 la cantidad de cangilones

    p[cont_PLANILLA].TIEMPO[cont_tiempo] = SEGUNDOS_ARRAY - TIEMPO_ANTERIOR; // mapea el tiempo de interrupción y le resta el tiempo anterior 
    TIEMPO_ANTERIOR = SEGUNDOS_ARRAY; // mapea el tiempo de interrupción en la variable tiempo_anterior para restar en la siguiente interrupción

    Serial.print("CANGILONES: "); // imprime "CANGILONES: "
    Serial.println(p[cont_PLANILLA].CANGILONES); // imprime la cantidad de cangilones hasta el momento
    Serial.print("SEGUNDOS: "); //  imprime "SEGUNDOS: "
    Serial.println(p[cont_PLANILLA].TIEMPO[cont_tiempo]); // imprime los segundos entre esta y la interrupción anterior
    Serial.print("POSICION DEL TIEMPO: "); // imprime "POSICION DEL TIEMPO: "
    Serial.println(cont_tiempo); // imprime la posicion en la que mapea, el tiempo entre interrupciones, del array en el typedef struct

    FLAG = 0; // reinicio de flag

    if (FLAG3) // INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO A
    {
      lcd.setCursor(15, 1); // posicion del cursor
      lcd.print(" "); // borro lo que estaba en la posicion de B
      lcd.setCursor(15, 0); // posicion del cursor
      lcd.print("A"); // Imprime en la posicion el lado A (es en el que está)
      Serial.println("LADO: A"); // Por puerto serie imprime el lado tambien en el que se encuentra
      Serial.println();
    }
    if (!FLAG3) //INTERRUPCION DEL FLAG 3, QUE ES EL QUE CAMBIA CADA VEZ QUE INTERRUMPE EL FLAG PARA MARCAR SI ESTA EN EL LADO B
    {
      lcd.setCursor(15, 0); // posicion del cursor
      lcd.print(" "); // borro lo que estaba en la posicion de B
      lcd.setCursor(15, 1); // posicion del cursor
      lcd.print("B"); // Imprime en la posicion el lado B (es en el que está)
      Serial.println("LADO: B"); // Por puerto serie imprime el lado tambien en el que se encuentra
      Serial.println();
    }

  }
}
//********************** LOOP ***********************//


void SERIAL_EVENT()
{
  while (Serial.available()) // lectura del teclado
  {
    int ESTADO = '0'; // 0 ASCII

    ESTADO = Serial.read(); // La variable estado pasara a valer lo que detecte del teclado

    if (ESTADO == '+') // cuando presione + pasará de planilla
    {
      cont_tiempo = 0; // reinicio del contador del array de p[].TIEMPO[cont_tiempo]

      p[cont_PLANILLA].CANGILONES = p[cont_PLANILLA].CANGILONES - 1; // resta 1 de los cangilones ya que el primer cangilon es de inicio, y de esa manera se puede sacar el tiempo promedio de todas las mediciones
      
      for (int i = 0; i < 30; i++) // suma todos los tiempos de cada cangilon
      {
       p[cont_PLANILLA].TIEMPO_PROMEDIO =  p[cont_PLANILLA].TIEMPO[i] + p[cont_PLANILLA].TIEMPO_PROMEDIO ; // suma el valor actual de la variable tiempo_promedio con el valor siguiente del tiempo
      }
      
      p[cont_PLANILLA].TIEMPO_PROMEDIO = p[cont_PLANILLA].TIEMPO_PROMEDIO / p[cont_PLANILLA].CANGILONES; // divide el tiempo total entre la cantidad de cangilones
      
      Serial.print("TIEMPO_PROMEDIO: "); // imprime "TIEMPO_PROMEDIO: "
      Serial.println (p[cont_PLANILLA].TIEMPO_PROMEDIO); // Imprime el valor del tiempo promedio
      Serial.println();

      cont_tiempo = cont_tiempo - 1; // le resta uno para q a la hora de mapear no comience en la posicion 1 del array
      
      cont_PLANILLA ++; // aumenta en 1 el contador de planillas 
      FLAG2 = 0; // reinicio del flag del timer por desbordamiento

      SEGUNDOS_ARRAY = 0; // reinicio de la variable segundos (es la q aumenta en uno cada vez que interrumpe el timer por desbordamiento)
      TIEMPO_ANTERIOR = 0; // reinicio de la variable tiempo anterior, asi se reinicia el sistema que calcula la diferencia entre el tiempo de interrupcion 

      Serial.println(); 
      Serial.print("CAMBIO A PLANILLA: "); // imprime "CAMBIO A PLANILLA: "
      Serial.println (cont_PLANILLA); // imprime el contador de planillas 
    }

    if (ESTADO == '-') // cuando presione - finalizará la medición
    { 
      p[cont_PLANILLA].CANGILONES = p[cont_PLANILLA].CANGILONES - 1; // resta 1 de los cangilones ya que el primer cangilon es de inicio en la planilla correspondiente en este caso el cont_PLANILLA valdría 9 ya que sería la décima medicion pero podría cambiar en caso de querer que sean mas, o menos
      cont_PLANILLA ++; // aumenta en 1 la posicion en la q esta el contador de planillas para poder sacar luego el promedio de  cangilones totales
      
      for (int i = 0; i < 10; i++) // suma en la variable PROM_CANGILONES todos los cangilones
      {
        PROM_CANGILONES = PROM_CANGILONES + p[i].CANGILONES; // suma los cangilones
      }

      PROM_CANGILONES = PROM_CANGILONES / cont_PLANILLA; // saca el promedio diviendolo por 10
      
      Serial.println();
      Serial.print("PROMEDIO DE CANGILONES: "); // imprime "PROMEDIO DE CANGILONES: "
      Serial.println(PROM_CANGILONES); // imprime el valor del promedio de los cangilones
      Serial.println("FINALIZACION DE PRUEBAS"); // imprime "FINALIZACION DE PRUEBAS"
    }
  }
}
//********************** LOOP ***********************//


//********************** SERVICIO DE INTERRUPCIÓN ***********************//
void AUMENTAR() 
{
  if ((millis() - INICIO_TIMER) > LIMITE) // antidebounce
  {
    INICIO_TIMER = millis(); // antidebounce
    FLAG = 1; // inicio del flag 1 (cuando interrumpe el cangión)
    FLAG2 = 1; // inicio del flag 2 (inicio del timer cuando interrumpe el cangilón)
  }
}
//********************** SERVICIO DE INTERRUPCIÓN ***********************//


//********************** SERVICIO DE INTERRUPCIÓN POR DESBORDAMIENTO ***********************//
// TIMER //
ISR(TIMER1_OVF_vect)
{
  TCNT1 = 0xC2F7;

  if (FLAG2) // cuando el FLAG2 == 1 
  {
    SEGUNDOS_ARRAY ++; // aumenta en uno los segundos
  }
}
//********************** SERVICIO DE INTERRUPCIÓN POR DESBORDAMIENTO ***********************//
